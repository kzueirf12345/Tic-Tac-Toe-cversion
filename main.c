#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { NONE = 0, CROSS = 1, ZERO = 2, DRAW = 3 } FieldType;
typedef enum { RANDOM = 0, EASY = 1, MEDIUM = 2, HARD = 3 } Difficulty;

#define true 1
#define false 0

typedef struct {
    FieldType* arr;
    int size;
} Vector;

typedef struct {
    Vector _batlefield;
    int _size;
    FieldType _turn;
    FieldType _player_type_field;
    int _player_turn;
    Difficulty _difficulty;
} Game;

void Game_Init(Game* game, int size, int player_turn, FieldType player_type_field,
               Difficulty difficulty) {
    game->_size = size;
    game->_player_turn = player_turn;
    game->_player_type_field = player_type_field;
    game->_difficulty = difficulty;
    game->_turn = (player_turn ? player_type_field : (FieldType)((player_type_field % 2) + 1));

    game->_batlefield.arr = ((FieldType*)malloc(size*size * sizeof(FieldType)));
    game->_batlefield.size = size;
    for (int i = 0; i < size * size; i++) {
        game->_batlefield.arr[i] = NONE;
    }
}

char VField(FieldType field) {
    switch (field) {
        case NONE:
            return ' ';
        case CROSS:
            return 'X';
        case ZERO:
            return 'O';
        default:
            return ' ';
    }
}

void Draw(const Vector* const vec) {
    for (int i = 0; i < vec->size; ++i) {
        for (int j = 0; j < vec->size; ++j) {
            printf(" —");
        }
        printf("\n|");
        for (int j = 0; j < vec->size; ++j) {
            printf("%d|", vec->size * i + j + 1);
        }
        printf("\n");
    }
    for (int j = 0; j < vec->size; ++j) {
        printf(" —");
    }
    printf("\n\n");
    for (int i = 0; i < vec->size; ++i) {
        for (int j = 0; j < vec->size; ++j) {
            printf(" —");
        }
        printf("\n|");
        for (int j = 0; j < vec->size; ++j) {
            printf("%c|", VField(vec->arr[i * vec->size + j]));
        }
        printf("\n");
    }
    for (int j = 0; j < vec->size; ++j) {
        printf(" —");
    }
    printf("\n");
}
/*
FieldType CheckEnd(const Vector* const vec) {
    int count_all = 0;
    for (int i = 0; i < vec->size; ++i) {
        int count_cross = 0, count_zero = 0;
        for (int j = 0; j < vec->size; ++j) {
            count_cross += (vec->arr[i][j] == CROSS);
            count_zero += (vec->arr[i][j] == ZERO);
        }
        if (count_cross == vec->size) {
            return CROSS;
        }
        if (count_zero == vec->size) {
            return ZERO;
        }
        count_all += count_zero + count_cross;
    }

    if (count_all == vec->size * vec->size) {
        return DRAW;
    }

    for (int j = 0; j < vec->size; ++j) {
        int count_cross = 0, count_zero = 0;
        for (int i = 0; i < vec->size; ++i) {
            count_cross += (vec->arr[i][j] == CROSS);
            count_zero += (vec->arr[i][j] == ZERO);
        }
        if (count_cross == vec->size) {
            return CROSS;
        }
        if (count_zero == vec->size) {
            return ZERO;
        }
    }

    int count_cross = 0, count_zero = 0;
    for (int i = 0; i < vec->size; ++i) {
        count_cross += (vec->arr[i][i] == CROSS);
        count_zero += (vec->arr[i][i] == ZERO);
    }
    if (count_cross == vec->size) {
        return CROSS;
    }
    if (count_zero == vec->size) {
        return ZERO;
    }

    count_cross = 0, count_zero = 0;
    for (int i = 0; i < vec->size; ++i) {
        count_cross += (vec->arr[vec->size - 1 - i][i] == CROSS);
        count_zero += (vec->arr[vec->size - 1 - i][i] == ZERO);
    }
    if (count_cross == vec->size) {
        return CROSS;
    }
    if (count_zero == vec->size) {
        return ZERO;
    }

    return NONE;
}
*/

FieldType CheckEnd(const Vector* const vec) {
    for (int row = 0; row < vec->size; ++row) {
        bool row_win = true;
        for (int col = 1; col < vec->size; ++col) {
            if (vec->arr[row*vec->size] != vec->arr[row * vec->size + col]) {
                row_win = false;
                break;
            }
        }
        if (row_win && vec->arr[row*vec->size] != NONE) {
            return vec->arr[row*vec->size];
        }
    }

    for (int col = 0; col < vec->size; ++col) {
        bool col_win = true;
        for (int row = 1; row < vec->size; ++row) {
            if (vec->arr[col] != vec->arr[row * vec->size + col]) {
                col_win = false;
                break;
            }
        }
        if (col_win && vec->arr[col] != NONE) {
            return vec->arr[col];
        }
    }

    bool diag_win = true;
    for (int i = 1; i < vec->size; ++i) {
        if (vec->arr[0] != vec->arr[i * vec->size + i]) {
            diag_win = false;
            break;
        }
    }
    if (diag_win && vec->arr[0] != NONE) {
        return vec->arr[0];
    }

    diag_win = true;
    for (int i = 1; i < vec->size; ++i) {
        if (vec->arr[vec->size - 1] != vec->arr[i * vec->size + vec->size - i - 1]) {
            diag_win = false;
            break;
        }
    }
    if (diag_win && vec->arr[vec->size - 1] != NONE) {
        return vec->arr[vec->size - 1];
    }

    for (int i = 0; i < vec->size * vec->size; ++i) {
        if (vec->arr[i] == NONE) {
            return NONE;
        }
    }

    return DRAW;
}

typedef struct {
    int* arr;
    int size;
} Changes;

Changes GetChanges(const Vector* const vec) {
    int count_none = 0;
    for (int i = 0; i < vec->size * vec->size; ++i) {
        if (vec->arr[i] == NONE) {
            ++count_none;
        }
    }

    int* changes = (int*)malloc(count_none * sizeof(int));

    for (int i = 0, k = 0; i < vec->size*vec->size; ++i) {
        if (vec->arr[i] == NONE) {
            changes[k++] = i + 1;
        }
    }

    Changes c = {changes, count_none};
    return c;
}
/*
FieldType BestResult(const Vector* const vec, int m, FieldType type) {
    FieldType checkend = CheckEnd(vec);
    if (checkend != NONE) return checkend;
    if (m == 0) return NONE;

    Changes p = GetChanges(vec);
    FieldType* arr = (FieldType*)malloc(p.size * sizeof(FieldType));

    for (int i = 0; i < p.size; ++i) {
        Vector newvec = {(FieldType**)malloc(vec->size * sizeof(FieldType*)), vec->size};

        for (int k = 0; k < vec->size; ++k) {
            newvec.arr[k] = (FieldType*)malloc(vec->size * sizeof(FieldType));
            for (int j = 0; j < vec->size; ++j) {
                newvec.arr[k][j] = vec->arr[k][j];
            }
        }

        newvec.arr[(p.arr[i] - 1) / vec->size][(p.arr[i] - 1) % vec->size] = type;
        arr[i] = BestResult(&newvec, m - 1, (type % 2) + 1);

        for (int k = 0; k < vec->size; ++k) {
            free(newvec.arr[k]);
        }
        free(newvec.arr);
    }

    FieldType ans = (m - 1) % 2 == 0 ? CROSS : ZERO;
    bool flag = false;
    for (int i = 0; i < p.size; ++i) {
        if ((m - 1) % 2 == 0 && arr[i] == ZERO) {
            ans = arr[i];
            break;
        } else if ((m - 1) % 2 != 0 && arr[i] == CROSS) {
            ans = arr[i];
            break;
        }

        if (arr[i] == DRAW) {
            ans = arr[i];
            flag = true;
        } else if (arr[i] == NONE && !flag) {
            ans = arr[i];
        }
    }

    free(p.arr);
    free(arr);
    return ans;
}
*/


FieldType BestResult(const Vector* const vec, int m, FieldType type) {
    FieldType checkend = CheckEnd(vec);
    if (checkend != NONE) return checkend;
    if (m == 0) return NONE;

    Changes p = GetChanges(vec);
    FieldType* arr = (FieldType*)malloc(p.size * sizeof(FieldType));

    Vector newvec = {(FieldType*)malloc(vec->size*vec->size * sizeof(FieldType)), vec->size};

    for (int i = 0; i < p.size; ++i) {
        memcpy(newvec.arr, vec->arr, vec->size*vec->size * sizeof(FieldType));

        newvec.arr[p.arr[i] - 1] = type;
        arr[i] = BestResult(&newvec, m - 1, (type % 2) + 1);
    }
    free(newvec.arr);

    FieldType ans = (m - 1) % 2 == 0 ? CROSS : ZERO;
    bool flag = false;
    for (int i = 0; i < p.size; ++i) {
        if ((m - 1) % 2 == 0 && arr[i] == ZERO) {
            ans = arr[i];
            break;
        } else if ((m - 1) % 2 != 0 && arr[i] == CROSS) {
            ans = arr[i];
            break;
        }

        if (arr[i] == DRAW) {
            ans = arr[i];
            flag = true;
        } else if (arr[i] == NONE && !flag) {
            ans = arr[i];
        }
    }

    free(p.arr);
    free(arr);
    return ans;
}

int GetRandomStep(const Vector* const vec) {
    Changes p = GetChanges(vec);
    int step = p.arr[rand() % p.size];
    free(p.arr);
    return step;
}

int GetStep(const Vector* const vec, int _difficulty, FieldType _player_type_field) {
    Changes p = GetChanges(vec);

    int depth = _difficulty == EASY     ? 2
                : _difficulty == MEDIUM ? /*(p.size / 2 % 2 == 1 ? p.size / 2 + 1 : p.size / 2)*/ 6
                                        : /*(p.size % 2 == 1 ? p.size / 2 + 1 : p.size)*/ 10;

    FieldType* arr = (FieldType*)malloc(p.size * sizeof(FieldType));

    Vector newvec = {(FieldType*)malloc(vec->size*vec->size * sizeof(FieldType)), vec->size};

    for (int i = 0; i < p.size; ++i) {
        memcpy(newvec.arr, vec->arr, vec->size * vec->size * sizeof(FieldType));

        newvec.arr[p.arr[i] - 1] =
            (_player_type_field % 2) + 1;
        arr[i] = BestResult(&newvec, depth, _player_type_field);
    }

    free(newvec.arr);

    int step = -1;
    for (int i = 0; i < p.size; ++i) {
        if (arr[i] == (_player_type_field % 2) + 1) {
            step = p.arr[i];
            break;
        }
    }
    if (step == -1) {
        for (int i = 0; i < p.size; ++i) {
            if (arr[i] == DRAW) {
                step = p.arr[i];
                break;
            }
        }
    }
    if (step == -1) {
        for (int i = 0; i < p.size; ++i) {
            if (arr[i] == NONE) {
                step = p.arr[i];
                break;
            }
        }
    }
    if (step == -1) {
        step = p.arr[0];
    }

    free(p.arr);
    free(arr);
    return step;
}

bool Step(Game* game, int step) {
    if (!game->_player_turn) {
        step = (game->_difficulty == 0
                    ? GetRandomStep(&game->_batlefield)
                    : GetStep(&game->_batlefield, game->_difficulty, game->_player_type_field));
    }
    --step;
    // const int row = step / game->_batlefield.size, col = step % game->_batlefield.size;
    if (game->_batlefield.arr[step] != NONE) {
        return false;
    }
    game->_player_turn = !game->_player_turn;
    game->_batlefield.arr[step] = game->_turn;
    game->_turn = (game->_turn % 2) + 1;
    return true;
}

int main() {
    srand((unsigned int)time(NULL));

    int size;
    printf("Enter the size of the field: ");
    scanf("%d", &size);

    printf("0. Random\n");
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");
    printf("Choose difficulty: ");
    int difficulty_num;
    scanf("%d", &difficulty_num);
    Difficulty difficulty = (Difficulty)difficulty_num;

    printf("Will you be the first to play? 'Y/y' or 'N/n': ");
    char ans_player_first;
    scanf(" %c", &ans_player_first);
    int player_first = (ans_player_first == 'Y' || ans_player_first == 'y');

    printf("Will you play for 'X/x' or for 'O/o'?: ");
    char ans_player_type_field;
    scanf(" %c", &ans_player_type_field);
    FieldType player_type_field =
        (ans_player_type_field == 'X' || ans_player_type_field == 'x') ? CROSS : ZERO;

    Game game;
    Game_Init(&game, size, player_first, player_type_field, difficulty);

    FieldType result;
    do {
        if (game._player_turn) {
            Draw(&game._batlefield);
            printf("Input Your Choise: ");
            int choice;
            scanf("%d", &choice);
            Step(&game, choice);
        } else {
            Step(&game, -1);
        }

    } while (!(result = CheckEnd(&game._batlefield)));
    Draw(&game._batlefield);

    if (result == DRAW) {
        printf("\nDraw!\n");
    } else if (result == player_type_field) {
        printf("You've won!\n");
    } else {
        printf("You've lost!\n");
    }
    return 0;
}
