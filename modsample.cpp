#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <stdio.h>

#include "config.h"
#include "waverom.h"

uint32_t n_sample = 18;

uint32_t playing_freq = 1;
uint32_t wav_position = 0;
bool stopped = false;

// Freq scale for sample playing
#define POS_SCALE 10

// Update PWM sample playing periodically
void pwm_interrupt_handler() {
  pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));

  if (stopped) {
    return;
  }

  if ((wav_position >> POS_SCALE) < SAMPLE_LENGTH - 1) {
    size_t sample_start = n_sample * SAMPLE_LENGTH;
    size_t audio_ptr = sample_start + (wav_position >> POS_SCALE);
    uint8_t audio_level = waverom[audio_ptr];

    if (audio_level == 0 && audio_ptr > sample_start) {
      stopped = true;
    } else {
      pwm_set_gpio_level(AUDIO_PIN, audio_level);
    }

    wav_position += playing_freq;
  } else {
    // reset to start
    wav_position = 0;
  }
}

// On MIDI receive
void on_uart_rx() {
  while (uart_is_readable(UART_ID)) {
    uint8_t command = uart_getc(UART_ID);
    if (command == 0x90) {
      // MIDI Note On
      uint8_t note = uart_getc(UART_ID);
      uint8_t velocity = uart_getc(UART_ID);
      if (velocity > 0) {
        // Yamaha uses velocity 0 as note off
        playing_freq = note_frequencies[note];
        wav_position = 0;
        stopped = false;
      }
    } else if (command == 0xB0) {
      // MIDI Control Change
      uint8_t cc = uart_getc(UART_ID);
      uint8_t value = uart_getc(UART_ID);
      if (cc == 16) {
        n_sample = value;
      }
    }
  }
}

int main() {
  set_sys_clock_khz(176000, true);

  // stdio_init_all();

  // Set up our UART with the required speed.
  uart_init(UART_ID, BAUD_RATE);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Set up a RX interrupt
  // We need to set up the handler first
  // Select correct interrupt for the UART we are using
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  // And set up and enable the interrupt handlers
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);

  // Now enable the UART to send interrupts - RX only
  uart_set_irq_enables(UART_ID, true, false);

  gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

  int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

  // Setup PWM interrupt to fire when PWM cycle is complete
  pwm_clear_irq(audio_pin_slice);
  pwm_set_irq_enabled(audio_pin_slice, true);
  // set the handle function above
  irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler);
  irq_set_enabled(PWM_IRQ_WRAP, true);

  // Setup PWM for audio output
  pwm_config config = pwm_get_default_config();
  /* Base clock 176,000,000 Hz divide by wrap 250 then the clock divider further
   * divides to set the interrupt rate.
   *
   * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
   *
   *
   * So clkdiv should be as follows for given sample rate
   *  8.0f for 11 KHz
   *  4.0f for 22 KHz
   *  2.0f for 44 KHz etc
   */
  pwm_config_set_clkdiv(&config, 8.0f);
  pwm_config_set_wrap(&config, 250);
  pwm_init(audio_pin_slice, &config, true);

  pwm_set_gpio_level(AUDIO_PIN, 0);

  while (1) {
    __wfi(); // Wait for Interrupt
  }
}
