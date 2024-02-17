// Copyright (c) 2023 Jonathan Engdahl
// BSD license -- see the accompanying LICENSE file

#ifndef SERIAL_H
#define SERIAL_H

extern bool ControlC;

extern bool __io_kbhit();
extern bool __io_txrdy();
extern int  __io_putchar(int ch);
extern int  __io_getchar();


#endif // SERIAL_H
