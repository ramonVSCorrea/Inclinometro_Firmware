// Configurações do servo motor
#define SERVO_PWM_CHANNEL LEDC_CHANNEL_0
#define SERVO_PWM_FREQ 50
#define SERVO_MAX_PULSE_WIDTH_US 2400
#define SERVO_MIN_PULSE_WIDTH_US 600

// Pino GPIO conectado ao servo motor
#define SERVO_GPIO_PIN 13

// Pino GPIO conectado ao botão
#define BUTTON GPIO_NUM_5

#define ANGLE_MIN 0
#define ANGLE_MAX 180

/**
 * @brief Função para iniciar o servo
 */
void servo_pwm_init();

/**
 * @brief Função que controla a inclinação do servo
 *
 * @param degrees Angulo que você deseja setar o servo
 */
void set_servo_position(int degrees);

/**
 * @brief Tarefa que controla a subida e descida de bascula
 */
void Task_Servo();