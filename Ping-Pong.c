#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <pthread.h>

#define WIDTH 40
#define HEIGHT 20
#define PADDLE_SIZE 3
#define BALL_SPEED 1
#define WIN_SCORE 5

int ball_x, ball_y;
int ball_dx = 1, ball_dy = 1;
int paddle1_y, paddle2_y;
int score1 = 0, score2 = 0;
int running = 1;
int game_over = 0;

pthread_mutex_t mutex;

void *game_logic(void *arg);
void *player_input(void *player);
void draw_screen();

int main()
{
    ball_x = WIDTH / 2;
    ball_y = HEIGHT / 2;
    paddle1_y = HEIGHT / 2 - PADDLE_SIZE / 2;
    paddle2_y = HEIGHT / 2 - PADDLE_SIZE / 2;

    pthread_t game_thread, player1_thread, player2_thread;
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&game_thread, NULL, game_logic, NULL);
    pthread_create(&player1_thread, NULL, player_input, (void *)"player1");
    pthread_create(&player2_thread, NULL, player_input, (void *)"player2");

    pthread_join(game_thread, NULL);
    pthread_join(player1_thread, NULL);
    pthread_join(player2_thread, NULL);

    pthread_mutex_destroy(&mutex);
    return 0;
}

void *game_logic(void *arg)
{
    while (running)
    {
        pthread_mutex_lock(&mutex);

        if (!game_over)
        {
            ball_x += ball_dx * BALL_SPEED;
            ball_y += ball_dy * BALL_SPEED;

            if (ball_y <= 0 || ball_y >= HEIGHT - 1)
            {
                ball_dy = -ball_dy;
            }

            if (ball_x == 1 && ball_y >= paddle1_y && ball_y <= paddle1_y + PADDLE_SIZE)
            {
                ball_dx = -ball_dx;
            }
            if (ball_x == WIDTH - 2 && ball_y >= paddle2_y && ball_y <= paddle2_y + PADDLE_SIZE)
            {
                ball_dx = -ball_dx;
            }

            if (ball_x <= 0)
            {
                score2++;
                ball_x = WIDTH / 2;
                ball_dx = 1;
            }
            if (ball_x >= WIDTH - 1)
            {
                score1++;
                ball_x = WIDTH / 2;
                ball_dx = -1;
            }

            if (score1 >= WIN_SCORE)
            {
                game_over = 1;
            }
            else if (score2 >= WIN_SCORE)
            {
                game_over = 2;
            }
        }

        draw_screen();
        pthread_mutex_unlock(&mutex);

        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            running = 0;
        }

        Sleep(50);
    }
    return NULL;
}

void *player_input(void *player)
{
    while (running)
    {
        pthread_mutex_lock(&mutex);

        if (!game_over)
        {
            if ((char *)player == "player1")
            {
                if (GetAsyncKeyState(VK_UP) & 0x8000 && paddle1_y > 0) {
                    paddle1_y--;
                }
                if (GetAsyncKeyState(VK_DOWN) & 0x8000 && paddle1_y < HEIGHT - PADDLE_SIZE) {
                    paddle1_y++;
                }
            }
            else
            {
                if (GetAsyncKeyState('W') & 0x8000 && paddle2_y > 0) {
                    paddle2_y--;
                }
                if (GetAsyncKeyState('S') & 0x8000 && paddle2_y < HEIGHT - PADDLE_SIZE) {
                    paddle2_y++;
                }
            }
        }

        pthread_mutex_unlock(&mutex);
        Sleep(30);
    }
    return NULL;
}

void draw_screen()
{
    system("cls");

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (x == 0 || x == WIDTH - 1) {
                printf("|");
            }
            else if (x == ball_x && y == ball_y) {
                printf("O");
            }
            else if (x == 1 && y >= paddle1_y && y < paddle1_y + PADDLE_SIZE) {
                printf("#");
            }
            else if (x == WIDTH - 2 && y >= paddle2_y && y < paddle2_y + PADDLE_SIZE) {
                printf("#");
            }
            else {
                printf(" ");
            }
        }
        printf("\n");
    }

    printf("\nPlayer 1 (arrow up/arrow down) Score: %d      Player 2 (w/s) Score: %d\n", score1, score2);

    if (game_over == 1)
    {
        printf("\nPlayer 1 Wins! Press ESC to exit...\n");
    }
    else if (game_over == 2)
    {
        printf("\nPlayer 2 Wins! Press ESC to exit...\n");
    }
}
