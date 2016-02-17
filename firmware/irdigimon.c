//------------------------------------------------------------
//
// Project : Digimon VPET Wristband
// �l�N�I�_�̘r�ɂ��Ă�����肽���Ƃ����v��
// ���̃t�@�C���͐ԊO����p������������H�̃v���O����
// �������Ƃ����Ă��M�A�̐M�����قڂ��̂܂ܐԊO��LED�ɗ����Ă��邾��
//
// file    : irdigimon.c
// version : 1.0
// created : 2016/02/06
// author  : botamochi
//
// MCU   : ATtiny10
// CLOCK : 1 MHz
// FUSE  : 0xFD
//------------------------------------------------------------
#include <avr/io.h>
#include <avr/wdt.h>

//------------------------------------------------------------
// �萔��`
//------------------------------------------------------------
#define IR_LED_PIN PB0
#define IR_RCV_PIN PB1
#define DM_SIG_PIN PB2

#define MODE_INPUT(pin)  DDRB &= ~(1<<(pin))
#define MODE_OUTPUT(pin) DDRB |= (1<<(pin))

#define BIT_SET(pin)  PORTB |= (1<<(pin))
#define BIT_CLR(pin)  PORTB &= ~(1<<(pin))
#define BIT_READ(pin) (PINB & (1<<(pin)))

#define IR_LED_ON()  DDRB |= (1<<IR_LED_PIN)
#define IR_LED_OFF() DDRB &= ~(1<<IR_LED_PIN)

#define RISING_EDGE(pin)  while (BIT_READ(pin) == 0)
#define FALLING_EDGE(pin) while (BIT_READ(pin) != 0)

//------------------------------------------------------------
// �֐��v���g�^�C�v
//------------------------------------------------------------
void initialize(void);
void ir2device(void);
void device2ir(void);

//------------------------------------------------------------
// main
//------------------------------------------------------------
int main(void)
{
  initialize();
  wdt_enable(WDTO_1S);
	
  while (1) {
    if (BIT_READ(IR_RCV_PIN) == 0) {
      ir2device();
    }
    if (BIT_READ(DM_SIG_PIN) == 0) {
      device2ir();
    }
    wdt_reset();
  }
  return 0;
}

//------------------------------------------------------------
// ������
//------------------------------------------------------------
void initialize()
{
  IR_LED_OFF();
  MODE_INPUT(IR_RCV_PIN);
  MODE_INPUT(DM_SIG_PIN);
  // 38kHz��PWM�M������
  TCCR0A = (1<<COM0A0);
  TCCR0B = (1<<WGM02) | (1<<CS00);
  OCR0A = 12;
}

//------------------------------------------------------------
// �ԊO������琬�M�A��
//------------------------------------------------------------
void ir2device()
{
  int i;

  MODE_OUTPUT(DM_SIG_PIN);
  BIT_CLR(DM_SIG_PIN);

  // ���Z�b�g�p���X
  RISING_EDGE(IR_RCV_PIN);
  BIT_SET(DM_SIG_PIN);

  // �X�^�[�g�p���X
  FALLING_EDGE(IR_RCV_PIN);
  BIT_CLR(DM_SIG_PIN);

  // �f�[�^�p���X
  for (i = 0; i < 16; i++) {
    RISING_EDGE(IR_RCV_PIN);
    BIT_SET(DM_SIG_PIN);
    FALLING_EDGE(IR_RCV_PIN);
    BIT_CLR(DM_SIG_PIN);
  }

  RISING_EDGE(IR_RCV_PIN);
  BIT_SET(DM_SIG_PIN);
  MODE_INPUT(DM_SIG_PIN);
}

//------------------------------------------------------------
// �琬�M�A����ԊO����
//------------------------------------------------------------
void device2ir()
{
  int i;

  IR_LED_ON();

  // ���Z�b�g�p���X
  RISING_EDGE(DM_SIG_PIN);
  IR_LED_OFF();

  // �X�^�[�g�p���X
  FALLING_EDGE(DM_SIG_PIN);
  IR_LED_ON();

  // �f�[�^�p���X
  for (i = 0; i < 16; i++) {
    RISING_EDGE(DM_SIG_PIN);
    IR_LED_OFF();
    FALLING_EDGE(DM_SIG_PIN);
    IR_LED_ON();
  }

  RISING_EDGE(DM_SIG_PIN);
  IR_LED_OFF();
  RISING_EDGE(IR_RCV_PIN); // ���g��LED�̔��ˌ��ɔ��������Ȃ�����
}
