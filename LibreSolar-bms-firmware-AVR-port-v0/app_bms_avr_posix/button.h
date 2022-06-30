/*
 * Copyright (c) The Libre Solar Project Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

/** @file
 *
 * @brief
 * Functions to handle on/off switch
 */

/**
 * Check if button was pressed for 3 seconds
 *
 * \return true if pressed for at least 3 seconds
 */
bool button_pressed_for_3s();
