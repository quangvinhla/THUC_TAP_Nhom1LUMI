#include "stm32f1xx_hal.h"
#include "lcd.h"         // Thư viện LCD, thay bằng OLED nếu dùng OLED
#include <stdlib.h>

#define LCD_WIDTH 16
#define LCD_HEIGHT 2

typedef struct {
    int x, y;
} Point;

#define MAX_SNAKE_LENGTH 32

Point snake[MAX_SNAKE_LENGTH];
int snake_length = 3;
int dir_x = 1, dir_y = 0;
Point food;
int level = 1; // 1 hoặc 2
uint32_t speed[] = {400, 200}; // ms delay cho mỗi level

// Hàm tạo mồi mới
void spawn_food() {
    food.x = rand() % LCD_WIDTH;
    food.y = rand() % LCD_HEIGHT;
}

// Hàm kiểm tra va chạm
int check_collision(int x, int y) {
    for (int i = 0; i < snake_length; i++) {
        if (snake[i].x == x && snake[i].y == y) return 1;
    }
    return 0;
}

// Hàm phát âm thanh (buzzer)
void beep() {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

// Xử lý nút nhấn điều khiển
void update_direction() {
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) { dir_x = 1; dir_y = 0; }
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) { dir_x = -1; dir_y = 0; }
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_SET) { dir_x = 0; dir_y = -1; }
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_SET) { dir_x = 0; dir_y = 1; }
}

// Hiển thị snake và food lên LCD
void draw() {
    lcd_clear();
    for (int i = 0; i < snake_length; i++) {
        lcd_gotoxy(snake[i].x, snake[i].y);
        lcd_putc('O');
    }
    lcd_gotoxy(food.x, food.y);
    lcd_putc('X');
}

// Hàm main game loop
void snake_game_loop() {
    spawn_food();
    while (1) {
        update_direction();

        // Di chuyển thân rắn
        for (int i = snake_length - 1; i > 0; i--) {
            snake[i] = snake[i-1];
        }
        snake[0].x += dir_x;
        snake[0].y += dir_y;

        // Kiểm tra thua
        if (snake[0].x < 0 || snake[0].x >= LCD_WIDTH ||
            snake[0].y < 0 || snake[0].y >= LCD_HEIGHT ||
            check_collision(snake[0].x, snake[0].y)) {
            beep(); // Âm thanh kết thúc
            lcd_clear();
            lcd_gotoxy(0,0);
            lcd_puts("Game Over!");
            break;
        }

        // Kiểm tra ăn mồi
        if (snake[0].x == food.x && snake[0].y == food.y) {
            beep(); // Âm thanh ăn mồi
            if (snake_length < MAX_SNAKE_LENGTH) snake_length++;
            spawn_food();
        }

        draw();
        HAL_Delay(speed[level-1]);
    }
}

// Trong hàm main()
int main(void) {
    HAL_Init();
    // Khởi tạo LCD, GPIO nút nhấn & buzzer
    lcd_init();
    // Khởi tạo các chân GPIO cho nút và buzzer (tùy phần cứng)

    // Khởi tạo rắn ban đầu
    snake[0] = (Point){5,0};
    snake[1] = (Point){4,0};
    snake[2] = (Point){3,0};

    // Lựa chọn level (có thể dùng nút nhấn chọn trước khi bắt đầu)
    level = 1; // hoặc 2

    snake_game_loop();
while (1); // Kết thúc game
}