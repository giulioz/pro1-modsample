#define UART_ID uart0
#define BAUD_RATE 31250

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define AUDIO_PIN 28

const uint16_t note_frequencies[] = {
    16,   17,   18,   19,   21,   22,   23,   25,   26,   28,   29,   31,
    33,   35,   37,   39,   41,   44,   46,   49,   52,   55,   58,   62,
    65,   69,   73,   78,   82,   87,   93,   98,   104,  110,  117,  123,
    131,  139,  147,  156,  165,  175,  185,  196,  208,  220,  233,  247,
    262,  277,  294,  311,  330,  349,  370,  392,  412,  440,  466,  494,
    523,  554,  587,  622,  659,  698,  740,  784,  830,  880,  932,  988,
    1047, 1109, 1175, 1245, 1318, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2217, 2349, 2489, 2637, 2794, 2960, 3520, 3729, 3951, 4186, 4435,
    4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902};

#define SAMPLE_LENGTH 8192
