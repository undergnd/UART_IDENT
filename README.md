UART_IDENT

PROGRAM FUNCTION

The program detects the UART baud rate based on the received byte and detects whether the transmitting module is supported, then sends an acknowledgment (supported / not supported) if the welcome byte structure is ok, otherwise it sends nothing.

RESULTS

The attached files /log measurement results.png, waveforms.png, logs_conf.png, waveforms_conf.png/ show the reception of the "0xA3" code with confirmation and the reception of a random character (bit period measurement pulse shown) for each speed in the range 9600 - 115200 bps. In the test, I changed the transmit speed in the TeraTerm or Putty terminal and received a response from the ARM controller at the calculated UART speed the same as the terminal transmit speed.

A SHORT DESCRIPTION OF HOW THE PROGRAM WORKS

The main function contains a while loop that can be interrupted with a dynamic timeout. Timeout is dynamic because its value is updated as the bit duration /min_period_value/ is found. Depending on the transmission speed, the bits have different widths. Based on the bit width measurement, the receiver is able to calculate the transmission speed /recruit_update_com_speed/ function. The bit width is measured by reading the T1 timer between two consecutive signal edges. Since we also know the slope of the edges /e.g.: int_edge = IRQ_EDGE_IS_SET_FALLING;/, we can determine whether the signal between them is ones or zeros. All read values ​​are written to the /struct Bits_s measurements[]/ array. After exceeding the timeout, individual bits are calculated based on the recorded durations of the measured sections between the edges /recruit_stop_measurement function/ and the byte value is calculated. The calculated byte is compared with the welcome bytes of the supported modules and if these bytes match, there is a positive response 0xAA. Otherwise, if the welcome byte structure is correct but the module is not supported, the response 0xAF is given. The response is sent at the current calculated speed.

The program works realistically, but it lacks various types of detection and protection against errors or unexpected events in transmission.
