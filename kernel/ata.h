/*
 * ============================================================================
 * ATA Disk Driver Header
 * ============================================================================
 * ATA PIO mode driver for reading sectors from disk
 * Used to load LLM model weights
 * ============================================================================
 */

#ifndef ATA_H
#define ATA_H

#include "kernel.h"

/* ATA I/O Ports (Primary bus) */
#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERROR        0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_STATUS       0x1F7
#define ATA_PRIMARY_COMMAND      0x1F7

/* ATA Commands */
#define ATA_CMD_READ_SECTORS     0x20
#define ATA_CMD_WRITE_SECTORS    0x30
#define ATA_CMD_IDENTIFY         0xEC

/* ATA Status bits */
#define ATA_STATUS_BSY           0x80    /* Busy */
#define ATA_STATUS_DRDY          0x40    /* Drive ready */
#define ATA_STATUS_DRQ           0x08    /* Data request */
#define ATA_STATUS_ERR           0x01    /* Error */

/* Sector size */
#define ATA_SECTOR_SIZE          512

/* Functions */
void ata_init(void);
int  ata_read_sectors(uint32_t lba, uint8_t count, void* buffer);
int  ata_read_bytes(uint32_t offset, uint32_t size, void* buffer);

/* Error codes */
#define ATA_SUCCESS              0
#define ATA_ERR_TIMEOUT         -1
#define ATA_ERR_READ            -2
#define ATA_ERR_NO_DRIVE        -3

#endif /* ATA_H */
