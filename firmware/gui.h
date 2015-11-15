/*
 * 4chord midi - LCD graphical user interface
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 */
#ifndef _GUI_H_
#define _GUI_H_

/**
 * Print the GUI on the display
 */
void gui_start(void);

/**
 * Print the startup logo on the display
 */
void gui_printlogo(void);

/**
 * Button press callback function for Menu Next button
 * @param arg Unused
 */
void gui_menu_next(void *arg);

/**
 * Button press callback function for Menu Previous button
 * @param arg Unused
 */
void gui_menu_prev(void *arg);

/**
 * Button press callback function for Menu Select button
 * @param arg Unused
 */
void gui_menu_select(void *arg);

#endif
