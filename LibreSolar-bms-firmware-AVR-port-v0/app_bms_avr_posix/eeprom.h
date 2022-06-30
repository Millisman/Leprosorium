/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once



/**
 * @file
 *
 * @brief Handling of internal or external EEPROM to store device configuration
 */

/**
 * Store current charge controller data to EEPROM
 */
void eeprom_store_data();

/**
 * Restore charge controller data from EEPROM and write to variables in RAM
 */
void eeprom_restore_data();
