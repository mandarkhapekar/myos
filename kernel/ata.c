/*
 * ============================================================================
 * ATA Disk Driver Implementation
 * ============================================================================
 * ATA PIO mode driver for reading sectors from disk.
 * Uses 28-bit LBA addressing (supports up to 128 GB).
 * 
 * Reference: https://wiki.osdev.org/ATA_PIO_Mode
 * ============================================================================
 */

#include "ata.h"
#include "screen.h"

/* Timeout for ATA operations (in iterations) */
#define ATA_TIMEOUT 100000

/* ============================================================================
 * Internal Functions
 * ============================================================================ */

/* Wait for BSY flag to clear */
static int ata_wait_bsy(void) {
    int timeout = ATA_TIMEOUT;
    while ((port_byte_in(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY) && timeout > 0) {
        timeout--;
    }
    return timeout > 0 ? ATA_SUCCESS : ATA_ERR_TIMEOUT;
}

/* Wait for DRQ flag to set (data ready) */
static int ata_wait_drq(void) {
    int timeout = ATA_TIMEOUT;
    uint8_t status;
    
    while (timeout > 0) {
        status = port_byte_in(ATA_PRIMARY_STATUS);
        
        if (status & ATA_STATUS_ERR) {
            return ATA_ERR_READ;
        }
        if (status & ATA_STATUS_DRQ) {
            return ATA_SUCCESS;
        }
        timeout--;
    }
    
    return ATA_ERR_TIMEOUT;
}

/* 400ns delay (read status port 4 times) */
static void ata_delay(void) {
    port_byte_in(ATA_PRIMARY_STATUS);
    port_byte_in(ATA_PRIMARY_STATUS);
    port_byte_in(ATA_PRIMARY_STATUS);
    port_byte_in(ATA_PRIMARY_STATUS);
}

/* ============================================================================
 * Public Functions
 * ============================================================================ */

/* Initialize ATA driver */
void ata_init(void) {
    /* Select primary master drive */
    port_byte_out(ATA_PRIMARY_DRIVE_HEAD, 0xA0);
    ata_delay();
    
    /* Soft reset - not strictly necessary but good practice */
    ata_wait_bsy();
}

/*
 * Read sectors using 28-bit LBA PIO mode
 * 
 * lba:    Starting sector number (0-indexed)
 * count:  Number of sectors to read (1-255, 0 means 256)
 * buffer: Destination buffer (must be at least count * 512 bytes)
 * 
 * Returns: ATA_SUCCESS or error code
 */
int ata_read_sectors(uint32_t lba, uint8_t count, void* buffer) {
    if (count == 0) count = 1;
    
    uint16_t* buf = (uint16_t*)buffer;
    
    /* Wait for drive to be ready */
    if (ata_wait_bsy() != ATA_SUCCESS) {
        return ATA_ERR_TIMEOUT;
    }
    
    /* Select drive and send high LBA bits */
    /* 0xE0 = master drive, LBA mode */
    port_byte_out(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    ata_delay();
    
    /* Send sector count */
    port_byte_out(ATA_PRIMARY_SECCOUNT, count);
    
    /* Send LBA address (low 24 bits) */
    port_byte_out(ATA_PRIMARY_LBA_LO, (uint8_t)(lba & 0xFF));
    port_byte_out(ATA_PRIMARY_LBA_MID, (uint8_t)((lba >> 8) & 0xFF));
    port_byte_out(ATA_PRIMARY_LBA_HI, (uint8_t)((lba >> 16) & 0xFF));
    
    /* Send read command */
    port_byte_out(ATA_PRIMARY_COMMAND, ATA_CMD_READ_SECTORS);
    
    /* Read sectors */
    for (int i = 0; i < count; i++) {
        /* Wait for data to be ready */
        if (ata_wait_drq() != ATA_SUCCESS) {
            return ATA_ERR_READ;
        }
        
        /* Read 256 words (512 bytes) */
        for (int j = 0; j < 256; j++) {
            *buf++ = port_word_in(ATA_PRIMARY_DATA);
        }
        
        ata_delay();
    }
    
    return ATA_SUCCESS;
}

/*
 * Read arbitrary bytes from disk
 * Handles sector alignment internally
 * 
 * offset: Byte offset from start of disk
 * size:   Number of bytes to read
 * buffer: Destination buffer
 * 
 * Returns: Number of bytes read, or negative error code
 */
int ata_read_bytes(uint32_t offset, uint32_t size, void* buffer) {
    uint8_t sector_buffer[ATA_SECTOR_SIZE];
    uint8_t* dest = (uint8_t*)buffer;
    uint32_t bytes_read = 0;
    
    while (size > 0) {
        uint32_t lba = offset / ATA_SECTOR_SIZE;
        uint32_t sector_offset = offset % ATA_SECTOR_SIZE;
        uint32_t bytes_in_sector = ATA_SECTOR_SIZE - sector_offset;
        uint32_t to_copy = (size < bytes_in_sector) ? size : bytes_in_sector;
        
        /* Read the sector */
        int result = ata_read_sectors(lba, 1, sector_buffer);
        if (result != ATA_SUCCESS) {
            return result;
        }
        
        /* Copy the relevant bytes */
        memcpy(dest, sector_buffer + sector_offset, to_copy);
        
        dest += to_copy;
        offset += to_copy;
        size -= to_copy;
        bytes_read += to_copy;
    }
    
    return bytes_read;
}
