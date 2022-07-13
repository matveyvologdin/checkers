#define _CRT_SECURE_NO_WARNINGS
#include "wincon.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <math.h>
#include <time.h>

#define WHITE 1
#define RED 2
#define SIZE_NAME 24
#define SIZE_INFO 4
#define RECORD_INFO_SIZE 25
#define RECORDS_FILE "records.txt"
#define LOG_FILE "log.txt"
#define HISTORY_FILE "history.txt"
#define INT_MIN -2147483648
#define NORMAL_DEPTH 4
#define HARD_DEPTH 6
#define MAX_COUNT_TURNS 28
//#define SHOW_TURN_AI
//#define TEST_ERRORS
#define ALL 0
#define ONE 1
#define MAIN 1
#define TMP 2
void main_menu();
void play_checkers(FILE*);
void help_info(int);
void init_from_file(FILE*, int*, int*);
void add_record(int, char*);
int choosing_dif();
void init_field();
void draw_field(COORD, COORD);
void draw_checkers(COORD, COORD);
void return_mark(COORD, int);
void print_model(COORD);
void print_queen(COORD);
void print_cell(COORD);
int have_to_cut(int, int);
int have_to_cut_cur(COORD, int);
void reverse(COORD*);
void eating(COORD);
void move(COORD, COORD, int);
void records();
void load_game();
void about();
int check_end(int, int);
void save_game();
void analyze(int);
// Äëÿ èñêóñòâåííîãî èíòåëëåêòà
void AI(int, int, COORD*, COORD*, int*, COORD[8]);
int minimax(int,int, int, COORD[MAX_COUNT_TURNS][2], int*, COORD[MAX_COUNT_TURNS][8]);
int alpha_beta(int, int, int, int, int, COORD[MAX_COUNT_TURNS][2], int*, COORD[MAX_COUNT_TURNS][8]);
int evaluate(int);
void move_forward(COORD*, COORD*, int, COORD[MAX_COUNT_TURNS][9], int, int[10]);
void move_back(COORD*, COORD*, int, COORD[9], int[10], int);
void eating_theory(COORD);
void find_moves(COORD[MAX_COUNT_TURNS][2], COORD[MAX_COUNT_TURNS][9], int, COORD[MAX_COUNT_TURNS][8]);
int is_weak(int, int);
int help_defend(int i, int j);
void deep_search(int, COORD, COORD[MAX_COUNT_TURNS][2], COORD[MAX_COUNT_TURNS][9], int*, COORD, int, COORD[MAX_COUNT_TURNS][8]);
void deep_search_for_queen(int, COORD, COORD[MAX_COUNT_TURNS][2], COORD[MAX_COUNT_TURNS][9], int*, COORD, int, COORD[MAX_COUNT_TURNS][8]);
COORD find_eated(int, COORD, COORD);

typedef enum check { no, white, red } check;

typedef struct cell
{
	COORD crd;
	check isBusy;
	COORD step[4];
	COORD cut[4];
	COORD step_q[4][7];
	COORD cut_q[4][6];
	bool isQueen;
}cell;
cell field[COUNT_FIELD][COUNT_FIELD];
cell field_tmp[COUNT_FIELD][COUNT_FIELD];

int main()
{
	// Èíèöèàëèçèðóåòñÿ êîíñîëü, ñêðûâàåòñÿ êóðñîð
	con_init();
	show_cursor(0);
	// Çàïóñê ãëàâíîãî ìåíþ
	main_menu();

	return 0;
}

// Ïîääåðæêà ãëàâíîãî ìåíþ
void main_menu()
{
	const char* menu_items[] = { "Èãðàòü", "Çàãðóçèòü èãðó", "Ðåêîðäû", "Î ïðîãðàììå", "Âûõîä" };
	int menu_active_idx = 0;
	int menu_items_count = sizeof(menu_items) / sizeof(menu_items[0]);

	short clr_bg = CON_CLR_BLUE;
	short clr_bg_active = CON_CLR_GRAY;
	short clr_font = CON_CLR_WHITE_LIGHT;
	int flag = 0;
	int last_act = 0;


	while (1)
	{
		int left = 50;
		int top = 9;
		int b;

		// Çàáëîêèðîâàòü îòðèñîâêó
		con_draw_lock();

		// Î÷èñòêà ýêðàíà
		con_set_color(clr_font, clr_bg);
		clrscr();
		
		// Öèêë îòðèñîâûâàåò êíîïêè
		for (b = 0; b < menu_items_count; b++)
		{
			short btn_bg = clr_bg; // Ïî óìîë÷àíèþ ôîí êíîïêè - êàê ôîí ýêðàíà
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // Åñëè êíîïêà àêòèâíà - òî ðèñóåòñÿ äðóãèì öâåòîì

			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);
			printf("--------------------");
			top++;
			gotoxy(left, top);
			printf("|                   ");
			
			gotoxy(left + 10 - strlen(menu_items[b]) / 2, top);
			printf("%s", menu_items[b]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 19, top);
			printf("|");
			top++;
			gotoxy(left, top);
			printf("--------------------");
			top += 2;
		}
		// Äàííûå ïîäãîòîâëåíû, âûâåñòè íà ýêðàí
		con_draw_release();
		

		while (!key_is_pressed()) // Åñëè ïîëüçîâàòåëü íàæèìàåò êíîïêó
		{
			int code = key_pressed_code();
			if (code == KEY_UP) // Åñëè ýòî ñòðåëêà ââåðõ
			{
				// Òî ïåðåõîä ê âåðõíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
				if (menu_active_idx > 0) 
				{
					last_act = 1;
					menu_active_idx--;
					break;
				}
			}
			else if (code == KEY_DOWN) // Åñëè ñòðåëêà âíèç
			{
				// Òî ïåðåõîä ê íèæíåìó ïóíêòó (åñëè ýòî âîçìîæíî)
				if (menu_active_idx + 1 < menu_items_count)
				{
					last_act = -1;
					menu_active_idx++;
					break;
				}
			}
			else if (code == KEY_ESC || code == 'q' || code == 'Q' || 
				     code == (unsigned char)'é' || code == (unsigned char)'É') // ESC èëè 'q' - âûõîä
			{
				last_act = 0;
				return;
			}
			else if (code == KEY_ENTER) // Íàæàòà êíîïêà Enter
			{
				last_act = 0;
				if (menu_active_idx == menu_items_count - 1) // Âûáðàí ïîñëåäíèé ïóíêò - ýòî âûõîä
					return;

				if (menu_active_idx == 0)
				{
					con_set_color(clr_font, clr_bg);
					clrscr();
					play_checkers(NULL);
				}

				if (menu_active_idx == 1)
					load_game();

				if (menu_active_idx == 2)
					records();

				if (menu_active_idx == menu_items_count - 2)
					about();

				break;
			}
		
		
			pause(40); // Íåáîëüíàÿ ïàóçà (÷òîáû íå çàãðóæàòü ïðîöåññîð)
		} // while (!key_is_pressed())


		// "Ñúåäàåòñÿ" îñòàâøèéñÿ ââîä
		while (key_is_pressed())
			key_pressed_code();

	} // while(1)
}


void about()
{
	con_set_color(CON_CLR_WHITE_LIGHT, CON_CLR_BLACK);
	clrscr();

	gotoxy(8, 2);
	printf("Î ïðîãðàììå:");

	con_set_color(CON_CLR_GRAY, CON_CLR_BLACK);
	gotoxy(8, 3);
	printf("Äàííàÿ ïðîãðàììà ÿâëÿåòñÿ êóðñîâîé ðàáîòîé ñòóäåíòîâ 1 êóðñà ÈÊèÇÈ ãð.4851003/00002\n");
	gotoxy(8, 4);
	printf("Âîëîãäèíà Ìàòâåÿ è Øåôàòîâà Âàäèìà\n\n");
	gotoxy(8, 6);
	printf("Äëÿ ïðîäîëæåíèÿ íàæìèòå ëþáóþ êëàâèøó.");
	
	key_pressed_code();
	return;
}

void records()
{
	char difficult[3][10] = { "Ëåãêî", "Íîðìàëüíî", "Ñëîæíî" };
	con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
	clrscr();
	FILE* records;
	records = fopen(RECORDS_FILE, "r+");
	if (records == NULL)
	{
		printf("Îøèáêà â îòêðûòèè ôàéëà");
		return;
	}
	char name[3][20][RECORD_INFO_SIZE];
	int count[3][20];
	memset(name, 0, sizeof(name));
	memset(count, 0, sizeof(count));
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 20; j++)
			fscanf(records, "%d%s", &count[i][j], name[i][j]);
	gotoxy(51, 3);
	printf("Òàáëèöà ðåêîðäîâ");
	gotoxy(48, 30);
	printf("Íàæìèòå q èëè ESC, ÷òîáû âûéòè");
	while (1)
	{
		int top = 4, left = 15;
		for (int j = 0; j < 3; j++)
		{
			left = 30 + 29 * j - strlen(difficult[j]) / 2;
			gotoxy(left, top);
			printf("%s", difficult[j]);
		}
		top++;
		left = 15;
		gotoxy(left, top);
		printf("----------------------------------------------------------------------------------------");
		top++;
		for (int j = 0; j < 3; j++)
		{
			gotoxy(left, top);
			printf("|Êîë-âî|                     ");
			left += 29;
		}
		printf("|");
		left = 15;
		top++;
		for (int j = 0; j < 3; j++)
		{
			gotoxy(left, top);
			printf("|ïîáåä |         Èìÿ         ");
			left += 29;
		}
		printf("|");
		top++;
		left = 15;
		int i = 0;
		while (i < 10)
			{
			gotoxy(left, top);
			printf("----------------------------------------------------------------------------------------");
			top++;
			for (int j = 0; j < 3; j++)
			{
				gotoxy(left, top);
				printf("|");
				gotoxy(left + 3 - (int)log10(count[j][i]) / 2, top);
				printf("%d", count[j][i]);
				left += 7;
				gotoxy(left, top);
				printf("|");
				gotoxy(left + 11 - strlen(name[j][i]) / 2, top);
				printf("%s", name[j][i]);
				left += 22;
			}
			gotoxy(left, top);
			printf("|");
			top++;
			left = 15;
			i++;
		}
		gotoxy(left, top);
		printf("----------------------------------------------------------------------------------------");
		while (!key_is_pressed())
		{
			int code = key_pressed_code();
			if (code > 127)
				code -= 256;
			if (code == 'q' || code == 'Q' || code == 'é' || code == 'É' || code == KEY_ESC)
				return;
		}
	}
}

void return_mark(COORD coord, int turn)
{
	con_set_color(CON_CLR_GRAY, CON_CLR_GRAY);
	print_cell(field[coord.X][coord.Y].crd);
	if (turn == white)
		con_set_color(CON_CLR_WHITE, CON_CLR_GRAY);
	else
		con_set_color(CON_CLR_RED, CON_CLR_GRAY);
	if (field[coord.X][coord.Y].isBusy != no && field[coord.X][coord.Y].isQueen == true)
		print_queen(field[coord.X][coord.Y].crd);
	else if (field[coord.X][coord.Y].isBusy != no)
		print_model(field[coord.X][coord.Y].crd);
}
void play_checkers(FILE* load)
{
	
	FILE* log;
	int flag = 0, d, dif, end;
	check turn = WHITE;
	clock_t begin_time = 0;
	if (load == NULL)
	{
		init_field();
		dif = choosing_dif();
		clock_t begin_time = clock();
		if (dif < 0)
			return;
		con_set_color(CON_CLR_BLACK, CON_CLR_BLUE);
		clrscr();
	}
	else
		init_from_file(load, &turn, &dif);
	help_info(dif);
	con_set_color(CON_CLR_BLACK, CON_CLR_BLUE);
	COORD cur, cur_n, move_old, move_new, for_cont, path[8], end_of_AI_turn = { -1,-1 },save_marked = { -1,-1 };
	memset(path, 0, sizeof(path));
	move_new.X = move_new.Y = move_old.X = move_old.Y = cur.X = cur.Y = for_cont.X = for_cont.Y = 0;
	memset(&cur_n, -1, sizeof(cur_n));
	draw_field(cur, cur_n);
	draw_checkers(cur, cur_n);
	cur.X = cur.Y = cur_n.X = cur_n.Y = 0;
	int n, k, is_multi, multi_process = 0;
	
	while (1)
	{
		end = check_end(turn, dif);
		if (end != 0)
		{
			clock_t end_time = clock();
			double time_spent = (double)(end_time - begin_time) / CLOCKS_PER_SEC;
			gotoxy(0, 0);
			FILE* result;
			double time = 0;
			if ((result = fopen("res.txt", "a+")) != NULL)
			{
				fprintf(result, "+%lf", time_spent);
				fclose(result);
			}
			break;
		}
		if (turn == white || turn == red)
		{
			if (turn == red)
			{
				save_marked = cur;
				if (multi_process == 0)
				{
					AI(turn, dif, &move_old, &move_new, &is_multi, path);
					if (is_multi == 1)
					{
						multi_process = 1;
						end_of_AI_turn = move_new;
						move_new = path[0];
						memcpy(path, &path[1], sizeof(&path[1]));
					}
				}
				else
				{
					move_old = move_new;
					move_new = path[0];
					memcpy(path, &path[1], sizeof(&path[1]));
					if (move_new.X == 0 && move_new.Y == 0)
					{
						move_new = end_of_AI_turn;
						multi_process = 0;
					}
				}
			}
			else
			{
				int c = key_pressed_code();
				if (c == 'q' || c == 'Q' || c == KEY_ESC || c == 'é' || c == 'É')
					return;
				if (c == 's' || c == 'û' || c == 'S' || c == 'Û')
				{
					save_game();
					con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
					clrscr();
					help_info(dif);
					draw_field(cur, (COORD) { -1, -1 });
					draw_checkers(cur, (COORD) { -1, -1 });
				}
				if (c == KEY_DOWN && cur.X < 7)
					cur_n.X++;
				if (c == KEY_UP && cur.X > 0)
					cur_n.X--;
				if (c == KEY_RIGHT && cur.Y < 7)
					cur_n.Y++;
				if (c == KEY_LEFT && cur.Y > 0)
					cur_n.Y--;
				if (c == KEY_ENTER && flag == 1)
				{
					turn = 3 - turn;
					flag = 0;
				}
				else if (flag == 1 && !(for_cont.X == 0 && for_cont.Y == 0))
				{
					move_old.X = for_cont.X;
					move_old.Y = for_cont.Y;
				}
				if (c == KEY_SPACE)
				{
					if (field[cur.X][cur.Y].isBusy == turn && move_old.X == 0 && move_old.Y == 0)
					{// Ñòàðòîâàÿ êëåòêà õîäà
						move_old.X = cur_n.X;
						move_old.Y = cur_n.Y;
					}
					else if (field[cur_n.X][cur_n.Y].isBusy == no && !(move_old.X == 0 && move_old.Y == 0))
					{// Êîíå÷íàÿ êëåòêà õîäà
						move_new.X = cur_n.X;
						move_new.Y = cur_n.Y;
					}
					else if (field[cur_n.X][cur_n.Y].isBusy == turn && !(move_old.X == 0 && move_old.Y == 0))
					{// Åñëè êîíå÷íàÿ êëåòêà ñ øàøêîé íàøåãî öâåòà, îíà âîñïðèíèìàåòñÿ êàê ñòàðòîâàÿ
						move_old.X = cur_n.X;
						move_old.Y = cur_n.Y;
					}
					else
					{
						memset(&move_old, 0, sizeof(move_old));
						memset(&move_new, 0, sizeof(move_new));
					}
				}
			}
				if (flag == 1 && for_cont.X == move_old.X && for_cont.Y == move_old.Y || flag == 0)// Åñëè ðóáêà ïðîäîëæàåòñÿ òî òîëüêî òîé æå øàøêîé
				{
					if (!(move_old.X == 0 && move_old.Y == 0) && !(move_new.X == 0 && move_new.Y == 0))//çàïèñàíû 2 ïîçèöèè
					{
						if (!have_to_cut(turn, 1) && field[move_old.X][move_old.Y].isQueen == false)//åñëè íå äîëæåí ðóáèòü, çíà÷èò õîä äëÿ øàøêè
						{
							for (int k = 0; k < 2; k++)
								if (field[move_old.X][move_old.Y].step[k].X == move_new.X && field[move_old.X][move_old.Y].step[k].Y == move_new.Y)//õîä íàïðàâî
								{
									move(move_old, move_new, field[move_old.X][move_old.Y].isBusy);
									analyze(MAIN);// Îáíîâèòü äîñòóïíûå õîäû
									turn = 3 - turn;
									if (turn == white)
										return_mark(save_marked, turn);
								}
						}
						else if (!have_to_cut(turn, 1))//èíà÷å õîä äëÿ äàìêè
						{
							for (int n = 0; n < 4; n++)
								for (int k = 0; k < 7; k++)
									if (field[move_old.X][move_old.Y].step_q[n][k].X == move_new.X && field[move_old.X][move_old.Y].step_q[n][k].Y == move_new.Y)
									{
										move(move_old, move_new, field[move_old.X][move_old.Y].isBusy);
										analyze(MAIN);// Îáíîâèòü äîñòóïíûå õîäû
										turn = 3 - turn;
										if (turn == white)
											return_mark(save_marked, turn);
									}
						}
						else if (field[move_old.X][move_old.Y].isQueen == false)//èíà÷å ðóáêà äëÿ øàøêè
						{
							for (int k = 0; k < 4; k++)
								if (field[move_old.X][move_old.Y].cut[k].X == move_new.X && field[move_old.X][move_old.Y].cut[k].Y == move_new.Y)
								{
									move(move_old, move_new, field[move_old.X][move_old.Y].isBusy);
									int incx, incy;
									if (move_new.X - move_old.X > 0)
										incx = 1;
									else
										incx = -1;
									if (move_new.Y - move_old.Y > 0)
										incy = 1;
									else
										incy = -1;
									move_old.X += incx;
									move_old.Y += incy;
									eating(move_old);
									flag = 1;
								}
							analyze(MAIN);// Îáíîâèòü äîñòóïíûå õîäû
							d = have_to_cut_cur(move_new, 1);// Ïðîâåðêà íà äîñòóïíîñòü ñðóáèòü åùå îäíó
							if (d == 0 && flag == 1)//åñëè ñðóáèë è áîëüøå íå ìîæåò
							{
								flag = 0;
								turn = 3 - turn;
								for_cont.X = 0;
								for_cont.Y = 0;
								if (turn == white)
									return_mark(save_marked, turn);
							}
							else if (d == 1)//åñëè ìîæåò ñðóáèòü åùå îäíó, ñîõðàíÿåì êîîðäèíàòû øàøêè, ÷òîáû õîäèòü èìåííî åé
								for_cont = move_new;
							if (d == 1 && move_new.X == end_of_AI_turn.X && move_new.Y == end_of_AI_turn.Y)//åñëè ìîæåò ðóáèòü, íî AI ðåøèë çàêîí÷èòü õîä ðàíüøå
							{
								turn = 3 - turn;
								if (turn == white)
									return_mark(save_marked, turn);
							}
						}
						else//èíà÷å ðóáêà äëÿ äàìêè
						{
							for (int n = 0; n < 4; n++)
								for (int k = 0; k < 6; k++)
									if (field[move_old.X][move_old.Y].cut_q[n][k].X == move_new.X && field[move_old.X][move_old.Y].cut_q[n][k].Y == move_new.Y)
									{
										move(move_old, move_new, field[move_old.X][move_old.Y].isBusy);
										int incx, incy;
										if (move_new.X - move_old.X > 0)
											incx = 1;
										else
											incx = -1;
										if (move_new.Y - move_old.Y > 0)
											incy = 1;
										else
											incy = -1;
										while (field[move_old.X][move_old.Y].isBusy != 3 - turn)
										{
											move_old.X += incx;
											move_old.Y += incy;
										}
										flag = 1;
										eating(move_old);
										analyze(MAIN);// Îáíîâèòü äîñòóïíûå õîäû
										break;
									}
							d = have_to_cut_cur(move_new, 1);//Ïðîâåðêà íà äîñòóïíîñòü ñðóáèòü åùå îäíó
							if (d == 0 && flag == 1)//åñëè ñðóáèë è áîëüøå íå ìîæåò
							{
								flag = 0;
								for_cont.X = 0;
								for_cont.Y = 0;
								turn = 3 - turn;
								if (turn == white)
									return_mark(save_marked, turn);
							}
							else if (d == 1)//åñëè ìîæåò ñðóáèòü åùå îäíó, ñîõðàíÿåì êîîðäèíàòû øàøêè, ÷òîáû õîäèòü èìåííî åé
								for_cont = move_new;
							if (d == 1 && move_new.X == end_of_AI_turn.X && move_new.Y == end_of_AI_turn.Y)//åñëè ìîæåò ðóáèòü, íî AI ðåøèë çàêîí÷èòü õîä ðàíüøå
							{
								turn = 3 - turn;
								if (turn == white)
									return_mark(save_marked, turn);
							}
						}
						log = fopen("log.txt", "w+");
						for (int i = 0; i < 8; i++)
						{
							for (int j = 0; j < 8; j++)
								fprintf(log, "%d%d\n", field[i][j].isBusy, field[i][j].isQueen);
						}
						fprintf(log, "%d\n", turn);
						fprintf(log, "%d\n", dif);
						fclose(log);
						if (turn == white)
						{
							memset(&move_old, 0, sizeof(move_old));
							memset(&move_new, 0, sizeof(move_new));
						}
					}
				}
		}
		if (cur_n.X != cur.X || cur_n.Y != cur.Y)//ñìåíà ôîêóñà
		{
			draw_field(cur, cur_n);
			draw_checkers(cur, cur_n);
			con_set_color(CON_CLR_WHITE,CON_CLR_BLUE);
			cur = cur_n;
#ifdef TEST_ERRORS
			int coord = 15, printed = 0;
			gotoxy(2, coord);
			if (field[cur.X][cur.Y].isBusy == turn && field[cur.X][cur.Y].isQueen == false)
			{
				int i = -1;
				printf("Äîñòóïíûå ðóáêè:");
				while (++i < 4)
				{
					if (!(field[cur.X][cur.Y].cut[i].X == 0 && field[cur.X][cur.Y].cut[i].Y == 0))
					{
						gotoxy(2, ++coord);
						printf("%d %d", field[cur.X][cur.Y].cut[i].X, field[cur.X][cur.Y].cut[i].Y);
						printed = 1;
					}
				}
				i = -1;
				gotoxy(2, ++coord);
				if (printed == 0)
				{
					printf("Äîñòóïíûå õîäû:");
					while (++i < 4)
					{
						if (!(field[cur.X][cur.Y].step[i].X == 0 && field[cur.X][cur.Y].step[i].Y == 0))
						{
							gotoxy(2, ++coord);
							printf("%d %d", field[cur.X][cur.Y].step[i].X, field[cur.X][cur.Y].step[i].Y);
						}
					}
				}
			}
			else if(field[cur.X][cur.Y].isBusy == turn && field[cur.X][cur.Y].isQueen == true)
			{
				printf("Äîñòóïíûå ðóáêè:");
				int i = -1, j = -1, printed = 0;
				while (++i < 4)
				{
					while (++j < 6)
					{
						if (field[cur.X][cur.Y].cut_q[i][j].X == 0 && field[cur.X][cur.Y].cut_q[i][j].Y == 0)
							break;
						else
						{
							gotoxy(2, ++coord);
							printf("%d %d", field[cur.X][cur.Y].cut_q[i][j].X, field[cur.X][cur.Y].cut_q[i][j].Y);
							printed = 1;
						}
					}
				}
				i = -1, j = -1;
				gotoxy(2, ++coord);
				if (printed == 0)
				{
					printf("Äîñòóïíûå õîäû:");
					while (++i < 4)
					{
						while (++j < 7)
						{
							if (field[cur.X][cur.Y].step_q[i][j].X == 0 && field[cur.X][cur.Y].step_q[i][j].Y == 0)
								break;
							else
							{
								gotoxy(2, ++coord);
								printf("%d %d", field[cur.X][cur.Y].step_q[i][j].X, field[cur.X][cur.Y].step_q[i][j].Y);
							}
						}
					}
				}
			}
			else
			{
				coord = 15;
				con_set_color(CON_CLR_BLUE, CON_CLR_BLUE);
				while (coord < 30)
				{
					gotoxy(2, coord++);
					printf("                  ");
				}
			}
#endif
		}
	}
}

void load_game()
{
	FILE* game;
	con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
	clrscr();
	char name[SIZE_NAME];
	gotoxy(50, 14);
	printf("Ââåäèòå èìÿ ñîõðàíåííîé èãðû:");
	gotoxy(60, 16);
	if (!scanf("%s", name))
		return;
	strcat(name, ".txt");
	game = fopen(name, "r+");
	if (game != NULL && strcmp(name, RECORDS_FILE) != 0 && strcmp(name, LOG_FILE) != 0)// Ñåéâ ñóùåñòâóåò è íàçâàíèå íå ñîâïàäàåò ñ âàæíûìè ôàéëàìè
		play_checkers(game);
	else
	{
		gotoxy(45, 18);
		printf("Èãðû ñ òàêèì èìåíåì åù¸ íå áûëî ñîõðàíåíî");
		pause(500);
	}
}

void init_field()
{
	memset(&field, 0, sizeof(cell) * COUNT_FIELD * COUNT_FIELD);
	for (int i = 0; i < COUNT_FIELD; i++)
	{
		for (int j = 0; j < COUNT_FIELD; j++)
		{
			
			field[i][j].crd.Y = 2 + i * 4;
			field[i][j].crd.X = 26 + j * 8;
			if (i < 3 && ((i % 2 == 0) && (j % 2 != 0) || (i % 2 != 0) && (j % 2 == 0)))
				field[i][j].isBusy = red;
			else if (i > 4 && (i % 2 != 0 && j % 2 == 0 || i % 2 == 0 && j % 2 != 0))
				field[i][j].isBusy = white;
			else
				field[i][j].isBusy = no;
			field[i][j].isQueen = false;
		}
	}
	analyze(MAIN);
}

void draw_field(COORD cur, COORD cur_n)
{
	int i = 0, j = 0, k = 0;
	if (cur_n.X == -1 && cur_n.Y == -1)//íà÷àëüíàÿ îòðèñîâêà âñåõ êëåòîê
	{
		for (i = 0; i < COUNT_FIELD; i++)
		{
			for (j = 0; j < COUNT_FIELD; j++)
			{
				if (i == cur.X && j == cur.Y)
					con_set_color(CON_CLR_GRAY, CON_CLR_GRAY);
				else if ((i + j) % 2 == 0)
					con_set_color(CON_CLR_WHITE, CON_CLR_WHITE);
				else
					con_set_color(CON_CLR_BLACK, CON_CLR_BLACK);
				print_cell(field[i][j].crd);
			}
		}
	}
	else if (cur.X >= 0 && cur.Y >= 0 && cur_n.X >= 0 && cur_n.Y >= 0)//îòðèñîâêà 2 èçìåíÿþùèõñÿ
	{
		con_set_color(CON_CLR_GRAY, CON_CLR_GRAY);
		print_cell(field[cur_n.X][cur_n.Y].crd);
		if ((cur.X % 2 == 0) && (cur.Y % 2 == 0) || (cur.X % 2 != 0) && (cur.Y % 2 != 0))
			con_set_color(CON_CLR_WHITE, CON_CLR_WHITE);
		else
			con_set_color(CON_CLR_BLACK, CON_CLR_BLACK);
		print_cell(field[cur.X][cur.Y].crd);
	}
}

void draw_checkers(COORD cur, COORD cur_n)
{
	if (cur_n.X == -1 && cur_n.Y == -1)//íà÷àëüíàÿ îòðèñîâêà âñåõ øàøåê
	{
		for (int i = 0; i < COUNT_FIELD; i++)
		{
			for (int j = 0; j < COUNT_FIELD; j++)
			{
				if (field[i][j].isBusy == red)
					con_set_color(CON_CLR_RED, CON_CLR_BLACK);
				else if (field[i][j].isBusy == white)
					con_set_color(CON_CLR_WHITE, CON_CLR_BLACK);
				else
					continue;
				if (i == cur.X && j == cur.Y && field[cur.X][cur.Y].isBusy == red)
					con_set_color(CON_CLR_RED, CON_CLR_GRAY);
				if (i == cur.X && j == cur.Y && field[cur.X][cur.Y].isBusy == white)
					con_set_color(CON_CLR_WHITE, CON_CLR_GRAY);
				if (!field[i][j].isQueen)
					print_model(field[i][j].crd);
				else
					print_queen(field[i][j].crd);
			}
		}
	}
	else//îòðèñîâêà 2 èçìåíÿþùèõñÿ
	{
		if ((cur.X % 2 == 0) && (cur.Y % 2 == 0) || (cur.X % 2 != 0) && (cur.Y % 2 != 0))//òåêóùàÿ êëåòêà ÷åðíàÿ
		{
			if (field[cur_n.X][cur_n.Y].isBusy == red)
			{
				con_set_color(CON_CLR_RED, CON_CLR_GRAY);
				if (field[cur_n.X][cur_n.Y].isQueen == true)
					print_queen(field[cur_n.X][cur_n.Y].crd);
				else
					print_model(field[cur_n.X][cur_n.Y].crd);
			}
			else if (field[cur_n.X][cur_n.Y].isBusy == white)
			{
				con_set_color(CON_CLR_WHITE, CON_CLR_GRAY);
				if (field[cur_n.X][cur_n.Y].isQueen == true)
					print_queen(field[cur_n.X][cur_n.Y].crd);
				else
					print_model(field[cur_n.X][cur_n.Y].crd);
			}
			
		}
		else//òåêóùàÿ êëåòêà áåëàÿ
		{
			if (field[cur.X][cur.Y].isBusy == red)
			{
				con_set_color(CON_CLR_RED, CON_CLR_BLACK);
				if (field[cur.X][cur.Y].isQueen == true)
					print_queen(field[cur.X][cur.Y].crd);
				else
					print_model(field[cur.X][cur.Y].crd);
			}
			else if (field[cur.X][cur.Y].isBusy == white)
			{
				con_set_color(CON_CLR_WHITE, CON_CLR_BLACK);
				if (field[cur.X][cur.Y].isQueen == true)
					print_queen(field[cur.X][cur.Y].crd);
				else
					print_model(field[cur.X][cur.Y].crd);
			}
		}
		
		
	}
}

void print_model(COORD crd)
{
	gotoxy(crd.X + 2, crd.Y);
	printf("____");
	gotoxy(crd.X + 1, crd.Y + 1);
	printf("/****\\");
	gotoxy(crd.X + 1, crd.Y + 2);
	printf("\\____/");
}

void print_queen(COORD crd)
{
	gotoxy(crd.X + 1, crd.Y);
	printf("|\\/\\/|");
	gotoxy(crd.X + 1, crd.Y + 1);
	printf("|****|");
	gotoxy(crd.X + 1, crd.Y + 2);
	printf("|____|");
}

void print_cell(COORD crd)
{
	int k = 0;
	while (k++ < 4)
	{
		gotoxy(crd.X, crd.Y++);
		printf("        ");
	}
}

void move(COORD start, COORD finish, int turn)
{
	FILE* history;
	history = fopen(HISTORY_FILE, "a+");
	fprintf(history, "%d%d --> %d%d turn - %d\n", start.X, start.Y, finish.X, finish.Y, turn);
	field[start.X][start.Y].isBusy = no;
	field[finish.X][finish.Y].isBusy = turn;
	con_set_color(CON_CLR_BLACK, CON_CLR_BLACK);
	print_cell(field[start.X][start.Y].crd);
	if (turn == white)
		//con_set_color(CON_CLR_WHITE, CON_CLR_GRAY);
		con_set_color(CON_CLR_WHITE, CON_CLR_BLACK);
	else
		con_set_color(CON_CLR_RED, CON_CLR_BLACK);
	print_cell(field[finish.X][finish.Y].crd);
	if (turn == white && finish.X == 0 || turn == red && finish.X == 7 || field[start.X][start.Y].isQueen == true)
	{
		print_queen(field[finish.X][finish.Y].crd);
		field[start.X][start.Y].isQueen = false;
		field[finish.X][finish.Y].isQueen = true;
	}
	else
		print_model(field[finish.X][finish.Y].crd);
	fclose(history);
}

int have_to_cut(int turn, int key)
{
	cell field_analyze[COUNT_FIELD][COUNT_FIELD];
	if (key == MAIN)
		memcpy(field_analyze, field, sizeof(field));
	else if (key == TMP)
		memcpy(field_analyze, field_tmp, sizeof(field_tmp));
	for (int i = 0; i < COUNT_FIELD; i++)
		for (int j = 0; j < COUNT_FIELD; j++)
		{
			if (field_analyze[i][j].isQueen == false)
			{
				for (int k = 0; k < 4; k++)
					if (!(field_analyze[i][j].cut[k].X == 0 && field_analyze[i][j].cut[k].Y == 0) && field_analyze[i][j].isBusy == turn)
					{
						if (key == MAIN)
							memcpy(field, field_analyze, sizeof(field_analyze));
						else if (key == TMP)
							memcpy(field_tmp, field_analyze, sizeof(field_analyze));
						return 1;
					}
			}
			else
				for (int k = 0; k < 4; k++)
					if (!(field_analyze[i][j].cut_q[k][0].X == 0 && field_analyze[i][j].cut_q[k][0].Y == 0) && field_analyze[i][j].isBusy == turn)
					{
						if (key == MAIN)
							memcpy(field, field_analyze, sizeof(field_analyze));
						else if (key == TMP)
							memcpy(field_tmp, field_analyze, sizeof(field_analyze));
						return 1;
					}
		}
	if (key == MAIN)
		memcpy(field, field_analyze, sizeof(field_analyze));
	else if (key == TMP)
		memcpy(field_tmp, field_analyze, sizeof(field_analyze));
	return 0;
}

int have_to_cut_cur(COORD crd, int key)
{
	cell field_analyze[COUNT_FIELD][COUNT_FIELD];
	if (key == MAIN)
		memcpy(field_analyze, field, sizeof(field));
	else if (key == TMP)
		memcpy(field_analyze, field_tmp, sizeof(field_tmp));
	if (!field_analyze[crd.X][crd.Y].isQueen)
	{
		for (int k = 0; k < 4; k++)
			if (!(field_analyze[crd.X][crd.Y].cut[k].X == 0 && field_analyze[crd.X][crd.Y].cut[k].Y == 0))
			{
				if (key == MAIN)
					memcpy(field, field_analyze, sizeof(field_analyze));
				else if (key == TMP)
					memcpy(field_tmp, field_analyze, sizeof(field_analyze));
				return 1;
			}
	}
	else
	{
		for (int k = 0; k < 4; k++)
			if (!(field_analyze[crd.X][crd.Y].cut_q[k][0].X == 0 && field_analyze[crd.X][crd.Y].cut_q[k][0].Y == 0))
			{
				if (key == MAIN)
					memcpy(field, field_analyze, sizeof(field_analyze));
				else if (key == TMP)
					memcpy(field_tmp, field_analyze, sizeof(field_analyze));
				return 1;
			}
	}
	if (key == MAIN)
		memcpy(field, field_analyze, sizeof(field_analyze));
	else if (key == TMP)
		memcpy(field_tmp, field_analyze, sizeof(field_analyze));
	return 0;
}
void eating(COORD crd)
{
	field[crd.X][crd.Y].isBusy = no;
	field[crd.X][crd.Y].isQueen = false;
	con_set_color(CON_CLR_BLACK, CON_CLR_BLACK);
	print_cell(field[crd.X][crd.Y].crd);
}

void analyze(int key)
{
	COORD crd;
	cell field_analyze[COUNT_FIELD][COUNT_FIELD];
	if (key == MAIN)
		memcpy(field_analyze, field, sizeof(field));
	else if (key == TMP)
		memcpy(field_analyze, field_tmp, sizeof(field_tmp));
	int turn, z;
	for (int i = 0; i < COUNT_FIELD; i++)
		for (int j = 0; j < COUNT_FIELD; j++)
		{
			crd.X = j;
			crd.Y = i;
			turn = field_analyze[i][j].isBusy;
			if (!field_analyze[i][j].isQueen)
			{
				if (crd.X > 0 && crd.Y > 0 && field_analyze[crd.Y - 1][crd.X - 1].isBusy == no && turn == white)//âëåâî ââåðõ õîä
				{
					field_analyze[crd.Y][crd.X].step[0].X = crd.Y - 1;
					field_analyze[crd.Y][crd.X].step[0].Y = crd.X - 1;
				}
				else if (turn == white || turn == no)
				{
					field_analyze[crd.Y][crd.X].step[0].X = 0;
					field_analyze[crd.Y][crd.X].step[0].Y = 0;
				}
				if (crd.X < 7 && crd.Y > 0 && field_analyze[crd.Y - 1][crd.X + 1].isBusy == no && turn == white)//âïðàâî ââåðõ õîä
				{
					field_analyze[crd.Y][crd.X].step[1].X = crd.Y - 1;
					field_analyze[crd.Y][crd.X].step[1].Y = crd.X + 1;
				}
				else if (turn == white || turn == no)
				{
					field_analyze[crd.Y][crd.X].step[1].X = 0;
					field_analyze[crd.Y][crd.X].step[1].Y = 0;
				}
				if (crd.X > 0 && crd.Y < 7 && field_analyze[crd.Y + 1][crd.X - 1].isBusy == no && turn == red)//âëåâî âíèç õîä
				{
					field_analyze[crd.Y][crd.X].step[0].X = crd.Y + 1;
					field_analyze[crd.Y][crd.X].step[0].Y = crd.X - 1;
				}
				else if (turn == red || turn == no)
				{
					field_analyze[crd.Y][crd.X].step[0].X = 0;
					field_analyze[crd.Y][crd.X].step[0].Y = 0;
				}
				if (crd.X < 7 && crd.Y < 7 && field_analyze[crd.Y + 1][crd.X + 1].isBusy == no && turn == red)//âïðàâî âíèç õîä
				{
					field_analyze[crd.Y][crd.X].step[1].X = crd.Y + 1;
					field_analyze[crd.Y][crd.X].step[1].Y = crd.X + 1;
				}
				else if (turn == red || turn == no)
				{
					field_analyze[crd.Y][crd.X].step[1].X = 0;
					field_analyze[crd.Y][crd.X].step[1].Y = 0;
				}
				if (crd.X > 1 && crd.Y > 1 && field_analyze[crd.Y - 1][crd.X - 1].isBusy == 3 - turn && field_analyze[crd.Y - 2][crd.X - 2].isBusy == no)//ðóáêà âëåâî ââåðõ
				{
					field_analyze[crd.Y][crd.X].cut[0].X = crd.Y - 2;
					field_analyze[crd.Y][crd.X].cut[0].Y = crd.X - 2;
				}
				else
				{
					field_analyze[crd.Y][crd.X].cut[0].X = 0;
					field_analyze[crd.Y][crd.X].cut[0].Y = 0;
				}
				if (crd.X < 6 && crd.Y > 1 && field_analyze[crd.Y - 1][crd.X + 1].isBusy == 3 - turn && field_analyze[crd.Y - 2][crd.X + 2].isBusy == no)//ðóáêà âïðàâî ââåðõ
				{
					field_analyze[crd.Y][crd.X].cut[1].X = crd.Y - 2;
					field_analyze[crd.Y][crd.X].cut[1].Y = crd.X + 2;
				}
				else
				{
					field_analyze[crd.Y][crd.X].cut[1].X = 0;
					field_analyze[crd.Y][crd.X].cut[1].Y = 0;
				}
				if (crd.X > 1 && crd.Y < 6 && field_analyze[crd.Y + 1][crd.X - 1].isBusy == 3 - turn && field_analyze[crd.Y + 2][crd.X - 2].isBusy == no)//ðóáêà âëåâî âíèç
				{
					field_analyze[crd.Y][crd.X].cut[2].X = crd.Y + 2;
					field_analyze[crd.Y][crd.X].cut[2].Y = crd.X - 2;
				}
				else
				{
					field_analyze[crd.Y][crd.X].cut[2].X = 0;
					field_analyze[crd.Y][crd.X].cut[2].Y = 0;
				}
				if (crd.X < 6 && crd.Y < 6 && field_analyze[crd.Y + 1][crd.X + 1].isBusy == 3 - turn && field_analyze[crd.Y + 2][crd.X + 2].isBusy == no)//ðóáêà âïðàâî âíèç
				{
					field_analyze[crd.Y][crd.X].cut[3].X = crd.Y + 2;
					field_analyze[crd.Y][crd.X].cut[3].Y = crd.X + 2;
				}
				else
				{
					field_analyze[crd.Y][crd.X].cut[3].X = 0;
					field_analyze[crd.Y][crd.X].cut[3].Y = 0;
				}
			}
			else//äàìêà
			{
				crd.X = j;
				crd.Y = i;
				z = 0;
				if (crd.X > 0 && crd.Y > 0)//õîä âëåâî ââåðõ
				{
					while (--crd.X >= 0 && --crd.Y >= 0)
					{
						if (field_analyze[crd.Y][crd.X].isBusy == no)
						{
							field_analyze[i][j].step_q[0][z].X = crd.Y;
							field_analyze[i][j].step_q[0][z].Y = crd.X;
							z++;
						}
						else
							break;
					}
				}
				if (z == 0)
					memset(field_analyze[i][j].step_q[0], 0, sizeof(field_analyze[i][j].step_q[0]));
				crd.X = j;
				crd.Y = i;
				z = 0;
				if (crd.X < 7 && crd.Y > 0)//õîä âïðàâî ââåðõ
				{
					while (++crd.X <= 7 && --crd.Y >= 0)
					{
						if (field_analyze[crd.Y][crd.X].isBusy == no)
						{
							field_analyze[i][j].step_q[1][z].X = crd.Y;
							field_analyze[i][j].step_q[1][z].Y = crd.X;
							z++;
						}
						else
							break;
					}
				}
				if (z == 0)
					memset(field_analyze[i][j].step_q[1], 0, sizeof(field_analyze[i][j].step_q[1]));
				crd.X = j;
				crd.Y = i;
				z = 0;
				if (crd.X > 0 && crd.Y < 7)//õîä âëåâî âíèç
				{
					while (--crd.X >= 0 && ++crd.Y <= 7)
					{
						if (field_analyze[crd.Y][crd.X].isBusy == no)
						{
							field_analyze[i][j].step_q[2][z].X = crd.Y;
							field_analyze[i][j].step_q[2][z].Y = crd.X;
							z++;
						}
						else
							break;
					}
				}
				if (z == 0)
					memset(field_analyze[i][j].step_q[2], 0, sizeof(field_analyze[i][j].step_q[2]));
				crd.X = j;
				crd.Y = i;
				z = 0;
				if (crd.X < 7 && crd.Y < 7)//õîä âïðàâî âíèç
				{
					while (++crd.X <= 7 && ++crd.Y <= 7)
					{
						if (field_analyze[crd.Y][crd.X].isBusy == no)
						{
							field_analyze[i][j].step_q[3][z].X = crd.Y;
							field_analyze[i][j].step_q[3][z].Y = crd.X;
							z++;
						}
						else
							break;
					}
				}
				if (z == 0)
					memset(field_analyze[i][j].step_q[3], 0, sizeof(field_analyze[i][j].step_q[3]));
				crd.X = j;
				crd.Y = i;
				z = 0;
				if (crd.X > 1 && crd.Y > 1)//ðóáêà âëåâî ââåðõ
				{
					while (--crd.Y > 0 && --crd.X > 0 && field_analyze[crd.Y][crd.X].isBusy != 3 - turn);
					if (crd.Y > 0 && crd.X > 0)
					{
						while (--crd.Y >= 0 && --crd.X >= 0)
						{
							if (field_analyze[crd.Y][crd.X].isBusy == no)
							{
								field_analyze[i][j].cut_q[0][z].X = crd.Y;
								field_analyze[i][j].cut_q[0][z].Y = crd.X;
								z++;
							}
							else
								break;
						}
					}
				}
				if (z == 0)
					memset(field_analyze[i][j].cut_q[0], 0, sizeof(field_analyze[i][j].cut_q[0]));
				z = 0;
				crd.X = j;
				crd.Y = i;
				if (crd.X < 6 && crd.Y > 1)//ðóáêà âïðàâî ââåðõ
				{
					while (--crd.Y > 0 && ++crd.X < 7 && field_analyze[crd.Y][crd.X].isBusy != 3 - turn);
					if (crd.Y > 0 && crd.X < 7)
						while (--crd.Y >= 0 && ++crd.X <= 7)
						{
							if (field_analyze[crd.Y][crd.X].isBusy == no)
							{
								field_analyze[i][j].cut_q[1][z].X = crd.Y;
								field_analyze[i][j].cut_q[1][z].Y = crd.X;
								z++;
							}
							else
								break;
						}
				}
				if (z == 0)
					memset(field_analyze[i][j].cut_q[1], 0, sizeof(field_analyze[i][j].cut_q[1]));
				z = 0;
				crd.X = j;
				crd.Y = i;
				if (crd.X > 1 && crd.Y < 6)//ðóáêà âëåâî âíèç
				{
					while (++crd.Y < 7 && --crd.X > 0 && field_analyze[crd.Y][crd.X].isBusy != 3 - turn);
					if (crd.Y < 7 && crd.X > 0)
						while (++crd.Y <= 7 && --crd.X >= 0)
						{
							if (field_analyze[crd.Y][crd.X].isBusy == no)
							{
								field_analyze[i][j].cut_q[2][z].X = crd.Y;
								field_analyze[i][j].cut_q[2][z].Y = crd.X;
								z++;
							}
							else
								break;
						}
				}
				if (z == 0)
					memset(field_analyze[i][j].cut_q[2], 0, sizeof(field_analyze[i][j].cut_q[2]));
				z = 0;
				crd.X = j;
				crd.Y = i;
				if (crd.X < 6 && crd.Y < 6)//ðóáêà âïðàâî âíèç
				{
					while (++crd.Y < 7 && ++crd.X < 7 && field_analyze[crd.Y][crd.X].isBusy != 3 - turn);
					if (crd.Y < 7 && crd.X < 7)
						while (++crd.Y <= 7 && ++crd.X <= 7)
						{
							if (field_analyze[crd.Y][crd.X].isBusy == no)
							{
								field_analyze[i][j].cut_q[3][z].X = crd.Y;
								field_analyze[i][j].cut_q[3][z].Y = crd.X;
								z++;
							}
							else
								break;
						}
				}
				if (z == 0)
					memset(field_analyze[i][j].cut_q[3], 0, sizeof(field_analyze[i][j].cut_q[3]));
				z = 0;
				crd.X = j;
				crd.Y = i;
			}
		}
		if (key == MAIN)
			memcpy(field, field_analyze, sizeof(field_analyze));
		else if (key == TMP)
			memcpy(field_tmp, field_analyze, sizeof(field_analyze));
}

int check_end(int turn, int dif)
{
	int cw = 0, cr = 0, stepw = 0, stepr = 0;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (field[i][j].isBusy == white)//Ïîäñ÷åò êîëè÷åñòâà äîñòóïíûõ øàøåê è äîñòóïíûõ õîäîâ äëÿ áåëûõ
			{
				cw++;//êîëè÷åñòâî øàøåê
				for (int n = 0; n < 4; n++)
				{
					if (!(field[i][j].step[n].X == 0 && field[i][j].step[n].Y == 0))
					{
						stepw++;//êîëè÷åñòâî ðóáîê
						break;
					}
					if (!(field[i][j].step_q[n][0].X == 0 && field[i][j].step_q[n][0].Y == 0))
					{
						stepw++;//êîëè÷åñòâî ðóáîê
						break;
					}
					if ((!(field[i][j].cut[n].X == 0 && field[i][j].cut[n].Y == 0)))
					{
						stepw++;//êîëè÷åñòâî ðóáîê
						break;
					}
					if (!(field[i][j].cut_q[n][0].X == 0 && field[i][j].cut_q[n][0].Y == 0))
					{
						stepw++;//êîëè÷åñòâî ðóáîê
						break;
					}
				}
			}
			else if (field[i][j].isBusy == red)//Ïîäñ÷åò êîëè÷åñòâà äîñòóïíûõ øàøåê è äîñòóïíûõ õîäîâ äëÿ êðàñíûõ
			{
				cr++;//êîëè÷åñòâî øàøåê
				for (int n = 0; n < 4; n++)
				{
					if (!(field[i][j].step[n].X == 0 && field[i][j].step[n].Y == 0))
					{
						stepr++;//êîëè÷åñòâî ðóáîê
						break;
					}
					if (!(field[i][j].step_q[n][0].X == 0 && field[i][j].step_q[n][0].Y == 0))
					{
						stepr++;//êîëè÷åñòâî ðóáîê
						break;
					}
					if ((!(field[i][j].cut[n].X == 0 && field[i][j].cut[n].Y == 0)))
					{
						stepr++;//êîëè÷åñòâî ðóáîê
						break;
					}
					if (!(field[i][j].cut_q[n][0].X == 0 && field[i][j].cut_q[n][0].Y == 0))
					{
						stepr++;//êîëè÷åñòâî ðóáîê
						break;
					}
				}
			}
		}
	con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
	if (cw == 0 || stepw == 0 && turn == white)
	{
		char name[SIZE_NAME];
		memset(name, 0, sizeof(name));
		gotoxy(0, 11);
		printf("Âû ïðîèãðàëè..\nÂâåäèòå ñâî¸ èìÿ:");
		gotoxy(0, 13);
		//scanf("%s", name);
		return 2;
	}
	else if (cr == 0 || stepr == 0 && turn == red)
	{
		char name[SIZE_NAME];
		memset(name, 0, sizeof(name));
		gotoxy(0, 11);
		printf("Âû âûèãðàëè!!!\nÂâåäèòå ñâî¸ èìÿ:");
		gotoxy(0, 13);
		//scanf("%s", name);
		//add_record(dif, name);
		return 1;
	}
	return 0;
}

int choosing_dif()
{
	con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
	clrscr();
	char difficult[3][10] = { "Ëåãêî", "Íîðìàëüíî", "Ñëîæíî" };
	int b = 0, menu_active_idx = 0;
	int clr_font = CON_CLR_WHITE, clr_bg_active = CON_CLR_GRAY, btn_bg = CON_CLR_BLUE;
	int flag = 0, last_act = 0;
	gotoxy(51, 13);
	printf("Âûáîð ñëîæíîñòè:");
	while (1)
	{
		b = 0;
		while (b < 3)// Îòðèñîâêà óðîâíåé ñëîæíîñòè
		{
			int left = 19 + 30 * b, top = 16;
			btn_bg = CON_CLR_BLUE;
			if (b == menu_active_idx)
				btn_bg = clr_bg_active; // Åñëè êíîïêà àêòèâíà - òî ðèñóåòñÿ äðóãèì öâåòîì

			gotoxy(left, top);
			con_set_color(clr_font, btn_bg);
			if (flag == 0 || (b == menu_active_idx || b == menu_active_idx + last_act) && flag == 1)
				printf("--------------------");
			top++;
			gotoxy(left, top);
			if (flag == 0 || (b == menu_active_idx || b == menu_active_idx + last_act) && flag == 1)
				printf("|                   ");

			gotoxy(left + 10 - strlen(difficult[b]) / 2, top);
			if (flag == 0 || (b == menu_active_idx || b == menu_active_idx + last_act) && flag == 1)
				printf("%s", difficult[b]);

			con_set_color(clr_font, btn_bg);
			gotoxy(left + 19, top);
			if (flag == 0 || (b == menu_active_idx || b == menu_active_idx + last_act) && flag == 1)
				printf("|");
			top++;
			gotoxy(left, top);
			if (flag == 0 || (b == menu_active_idx || b == menu_active_idx + last_act) && flag == 1)
				printf("--------------------");
			top += 2;
			b++;
		}
		if (flag == 0)
			flag = 1;
		while (!key_is_pressed()) // Åñëè ïîëüçîâàòåëü íàæèìàåò êíîïêó
		{
			int code = key_pressed_code();
			if (code == KEY_ENTER)
				return menu_active_idx;
			else if (code == KEY_RIGHT)
			{
				if (menu_active_idx < 2)
				{
					last_act = -1;
					menu_active_idx++;
					break;
				}
			}
			else if (code == KEY_LEFT)
			{
				if (menu_active_idx > 0)
				{
					last_act = 1;
					menu_active_idx--;
					break;
				}
			}
			else if (code == KEY_ESC)
				return -2;
		}
	}
	return -1;
}

void save_game()
{
	FILE* log;
	con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
	gotoxy(0, 5);
	char name[SIZE_NAME];
	char info[SIZE_INFO];
	memset(name, 0, sizeof(name));
	printf("Ââåäèòå íàçâàíèå ôàéëà:");
	gotoxy(0, 6);
	if (!scanf("%s", name))
		return;
	strcat(name, ".txt");
	FILE* save = fopen(name, "w+");
	log = fopen("log.txt", "r+");
	int i = 0;
	while (i < 66)
	{
		fgets(info, 10, log);
		fputs(info, save);
		i++;
	}
	printf("Èãðà óñïåøíî ñîõðàíåíà!");
	//pause(2000);
	fclose(log);
	fclose(save);
}

void help_info(int dif)
{
	char difficult[3][10] = { "Ëåãêî", "Íîðìàëüíî", "Ñëîæíî" };
	con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
	gotoxy(0, 1);
	printf("Íàæìèòå q èëè ESC äëÿ âûõîäà áåç ñîõðàíåíèÿ");
	gotoxy(0, 3);
	printf("Íàæìèòå s, ÷òîáû ñîõðàíèòü èãðó");
	gotoxy(4, 9);
	printf("Òåêóùàÿ ñëîæíîñòü:");
	gotoxy(13 - strlen(difficult[dif]) / 2, 10);
	printf("%s", difficult[dif]);
}

void init_from_file(FILE* load, int* turn, int* dif)
{
	con_set_color(CON_CLR_WHITE, CON_CLR_BLUE);
	clrscr();
	char info[SIZE_INFO];
	memset(info, 0, sizeof(info));
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			fgets(info, SIZE_INFO, load);
			field[i][j].isBusy = info[0] - 48;
			field[i][j].isQueen = info[1] - 48;
			field[i][j].crd.Y = 2 + i * 4;
			field[i][j].crd.X = 26 + j * 8;
		}
	fgets(info, SIZE_INFO, load);
	*turn = info[0] - 48;
	fgets(info, SIZE_INFO, load);
	*dif = info[0] - 48;
	analyze(MAIN);
	return;
}

void add_record(int dif, char* player)
{
	FILE* records = fopen(RECORDS_FILE, "r+");
	char name[3][20][SIZE_NAME];
	int count[3][20], sj;
	memset(name, 0, sizeof(name));
	memset(count, 0, sizeof(count));
	int flag = 0, i, j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 20; j++)
		{
			fscanf(records, "%d%s", &count[i][j], name[i][j]);
			if (strcmp(name[i][j], player) == 0 && i == dif && flag == 0)
			{
				count[i][j]++;
				sj = j;
				flag = 1;
			}
		}
	if (flag == 0)// Íå íàøëè â òàáëèöå
	{
		j = 19;
		while (j > -1 && count[dif][j] == 0)
			j--;
		j++;
		count[dif][j] = 1;
		strcpy(name[dif][j], player);
	}
	else // Íàøëè â òàáëèöå
	{

		while (sj > 0 && count[dif][sj] > count[dif][sj - 1])
		{
			int count_tmp = count[dif][sj];
			count[dif][sj] = count[dif][sj - 1];
			count[dif][sj - 1] = count_tmp;
			char name_tmp[SIZE_NAME];
			memset(name_tmp, 0, sizeof(name_tmp));
			strcpy(name_tmp, name[dif][sj]);
			strcpy(name[dif][sj], name[dif][sj - 1]);
			strcpy(name[dif][sj - 1], name_tmp);
			sj--;
		}
	}
	fclose(records);
	records = fopen(RECORDS_FILE, "w+");
	for (i = 0; i < 3; i++)
		for (j = 0; j < 20; j++)
			fprintf(records,"%d %s\n", count[i][j], name[i][j]);
	fclose(records);
}

void AI(int turn, int dif,COORD* crd, COORD* crd_new, int* is_multi, COORD path[8])
{
	int count = 0, result, i, j, n, k;
	if (dif == 0)
	{
		if (have_to_cut(turn, 1))// Ïîèñê äîñòóïíûõ ðóáîê
		{
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 0)
						for (k = 0; k < 4; k++)
							if (!(field[i][j].cut[k].X == 0 && field[i][j].cut[k].Y == 0))
								count++;
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 1)
						for (k = 0; k < 4; k++)
							for (n = 0; n < 6; n++)
								if (!(field[i][j].cut_q[k][n].X == 0 && field[i][j].cut_q[k][n].Y == 0))
									count++;
				}
			srand(time(NULL));
			result = rand() % count + 1;
			count = 0;
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 0)
						for (k = 0; k < 4; k++)
							if (!(field[i][j].cut[k].X == 0 && field[i][j].cut[k].Y == 0))
							{
								count++;
								if (count == result)
								{
									crd->X = i;
									crd->Y = j;
									crd_new->X = field[i][j].cut[k].X;
									crd_new->Y = field[i][j].cut[k].Y;
									return;
								}
							}
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 1)
						for (k = 0; k < 4; k++)
							for (n = 0; n < 6; n++)
								if (!(field[i][j].cut_q[k][n].X == 0 && field[i][j].cut_q[k][n].Y == 0))
								{
									count++;
									if (count == result)
									{
										crd->X = i;
										crd->Y = j;
										crd_new->X = field[i][j].cut_q[k][n].X;
										crd_new->Y = field[i][j].cut_q[k][n].Y;
										return;
									}
								}
				}
		}
		else// Ïîèñê äîñòóïíûõ õîäîâ
		{
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 0)
						for (k = 0; k < 4; k++)
							if (!(field[i][j].step[k].X == 0 && field[i][j].step[k].Y == 0))
								count++;
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 1)
						for (k = 0; k < 4; k++)
							for (n = 0; n < 6; n++)
								if (!(field[i][j].step_q[k][n].X == 0 && field[i][j].step_q[k][n].Y == 0))
									count++;
				}
			srand(time(NULL));
			result = rand() % count + 1;
			count = 0;
			for (i = 0; i < 8; i++)
				for (j = 0; j < 8; j++)
				{
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 0)
						for (k = 0; k < 4; k++)
							if (!(field[i][j].step[k].X == 0 && field[i][j].step[k].Y == 0))
							{
								count++;
								if (count == result)
								{
									crd->X = i;
									crd->Y = j;
									crd_new->X = field[i][j].step[k].X;
									crd_new->Y = field[i][j].step[k].Y;
									return;
								}
							}
					if (field[i][j].isBusy == turn && field[i][j].isQueen == 1)
						for (k = 0; k < 4; k++)
							for (n = 0; n < 6; n++)
								if (!(field[i][j].step_q[k][n].X == 0 && field[i][j].step_q[k][n].Y == 0))
								{
									count++;
									if (count == result)
									{
										crd->X = i;
										crd->Y = j;
										crd_new->X = field[i][j].step_q[k][n].X;
										crd_new->Y = field[i][j].step_q[k][n].Y;
										return;
									}
								}
				}
		}
	}
	else if (dif == 1)
	{
		COORD save_move[MAX_COUNT_TURNS][2], path1[MAX_COUNT_TURNS][8];// path1 ñîäåðæèò âñå ïåðåõîäíûå òî÷êè äëÿ êàæäîãî õîäà(åñëè øàøêà ñðóáàåò áîëüøå ÷åì îäíó)
		memset(path1, 0, sizeof(path1));
		memset(save_move, 0, sizeof(save_move));
		int n = 0, act;
		memcpy(field_tmp, field, sizeof(field));
		act = minimax(turn, turn, NORMAL_DEPTH, &save_move, &n, path1);//Ïðîñ÷åò èäåàëüíîãî õîäà, âîçâðàùàåò èíäåêñ õîäà â ìàññèâå âñåõ õîäîâ
		*crd = save_move[n][0];//Íà÷àëî õîäà
		*crd_new = save_move[n][1];// Êîíåö õîäà
		if (!(path1[act][0].X == 0 && path1[act][0].Y == 0))//Åñëè íåñêîëüêî ðóáîê îäíîé øàøêîé, òî is_multi = 1
			*is_multi = 1;
		else
			*is_multi = 0;
		memcpy(path, path1[act], sizeof(path));//Âåñü ïóòü êîòîðûé øàøêà ïðîõîäèò, çàïèñûâàåì âî âõîäÿùóþ ïåðåìåííóþ
	}
	else if (dif == 2)
	{
		COORD save_move[MAX_COUNT_TURNS][2], path1[MAX_COUNT_TURNS][8];//path1 ñîäåðæèò âñå ïåðåõîäíûå òî÷êè äëÿ êàæäîãî õîäà(åñëè øàøêà ñðóáàåò áîëüøå ÷åì îäíó)
		memset(path1, 0, sizeof(path1));
		memset(save_move, 0, sizeof(save_move));
		int n = 0, act;
		memcpy(field_tmp, field, sizeof(field));
		act = alpha_beta(turn, turn, HARD_DEPTH, -10000, 10000, &save_move, &n, path1);//Ïðîñ÷åò èäåàëüíîãî õîäà, âîçâðàùàåò èíäåêñ õîäà â ìàññèâå âñåõ õîäîâ
		*crd = save_move[n][0];//Íà÷àëî õîäà
		*crd_new = save_move[n][1];// Êîíåö õîäà
		if (!(path1[act][0].X == 0 && path1[act][0].Y == 0))//Åñëè íåñêîëüêî ðóáîê îäíîé øàøêîé, òî is_multi = 1
			*is_multi = 1;
		else
			*is_multi = 0;
		memcpy(path, path1[act], sizeof(path));//Âåñü ïóòü êîòîðûé øàøêà ïðîõîäèò, çàïèñûâàåì âî âõîäÿùóþ ïåðåìåííóþ
	}
}

int minimax(int global_turn, int turn, int depth, COORD save_move[MAX_COUNT_TURNS][2], int* n, COORD path[MAX_COUNT_TURNS][8])
{
	if (depth == 0) return evaluate(turn);//îöåíêà ïîçèöèè

	int score = -10000, is_cut = 0, was_queen[10], op_turn = 3 - turn, value = 0;
	if (depth % 2 == 0)//äëÿ ÷åòíûõ èùåì ìàêñèìóì, äëÿ íå÷åòíûõ - ìèíèìóì
		value = -10000;
	else
		value = 10000;
	COORD moves[MAX_COUNT_TURNS][2], eated_checker[MAX_COUNT_TURNS][9], path1[MAX_COUNT_TURNS][8];
	memset(eated_checker, 0, sizeof(eated_checker));
	memset(moves, 0, sizeof(moves));
	memset(was_queen, 0, sizeof(was_queen));
	memset(path1, 0, sizeof(path1));

	analyze(TMP);
	find_moves(moves, eated_checker, turn, path);//ïðîñ÷åò óâñåõ äîñòóïíûõ õîäîâ
	memcpy(path1, path, sizeof(path1));
	int i = 0;
	while (!(moves[i][0].X == 0 && moves[i][0].Y == 0) && !(moves[i][1].X == 0 && moves[i][1].Y == 0))
	{//ïîêà åñòü äîñòóïíûå õîäû
		move_forward(&moves[i][0], &moves[i][1], turn, eated_checker, i, was_queen);
		int tmp = minimax(global_turn, op_turn, depth - 1, save_move, n, path);
		move_back(&moves[i][0], &moves[i][1], turn, &eated_checker[i], was_queen, ALL);
		was_queen[9] = 0;// [9] ïîêàçûâàåò äàìêà ëè øàøêà, êîòîðàÿ õîäèò

		if (depth == NORMAL_DEPTH && tmp > value)// åñëè íàõîäèìñÿ íà âåðøèíå è áîëüøå óæå íàéäåííîãî, çíà÷èò çàïèñûâàåì
		{
			(*n) = 0;
			memset(save_move, 0, sizeof(COORD) * 56);
			save_move[*n][0] = moves[i][0];
			save_move[*n][1] = moves[i][1];
			(*n)++;
			value = tmp;
		}
		else if (depth == NORMAL_DEPTH && tmp == value)// åñëè íàõîäèìñÿ íà âåðøèíå è ðàâíî íàèáîëüøåìó, äîïèñûâàåì â ìàññèâ èäåàëüíûõ õîäîâ
		{
			save_move[*n][0] = moves[i][0];
			save_move[*n][1] = moves[i][1];
			(*n)++;
		}
		else if (depth % 2 == 0 && tmp > value || depth % 2 != 0 && tmp < value)// åñëè ëó÷øå, ÷åì óæå íàéäåííûé(max/min), òî çàïîìíèì åãî
			value = tmp;
		i++;
	}
	if (depth == NORMAL_DEPTH)//îêîí÷àòåëüíàÿ îáðàáîòêà ðåçóëüòàòà
	{
		memcpy(path, path1, sizeof(path1));
		int act;
		int g = 1;
		while (!(save_move[g][0].X == 0 && save_move[g][0].Y == 0))//ñ÷èòàåì êîëè÷åñòâî ëó÷øèõ õîäîâ
			g++;
		srand(time(NULL));
		act = rand() % g;// åñëè èõ íåñêîëüêî, âûáèðàåì ðàíäîìîì
		g = 0;
		while (!(save_move[act][0].X == moves[g][0].X && save_move[act][0].Y == moves[g][0].Y && save_move[act][1].X == moves[g][1].X && save_move[act][1].Y == moves[g][1].Y))
			g++;// èùåì èíäåêñ èäåàëüíîãî õîäà â ìàññèâå õîäîâ
		(*n) = act;
		return g;//âîçâðàùàåì òîò ñàìûé èíäåêñ
	}
	if (moves[0][0].X == 0 && moves[0][0].Y == 0 && moves[0][1].X == 0 && moves[0][1].Y == 0 && turn != global_turn)
		return 200 + depth;//åñëè õîäîâ áåëûõ íåò, çíà÷èò ïîáåäà äëÿ êðàñíûõ è äàåì î÷åíü áîëüøîå êîë-âî î÷êîâ
	if (moves[0][0].X == 0 && moves[0][0].Y == 0 && moves[0][1].X == 0 && moves[0][1].Y == 0 && turn == global_turn)
		return -200 + depth;//åñëè õîäîâ êðàñíûõ íåò, çíà÷èò ïîáåäà äëÿ áåëûõ è äàåì î÷åíü ìàëåíüêîå êîë-âî î÷êîâ
	return value;
}

int alpha_beta(int global_turn, int turn, int depth, int alpha, int beta, COORD save_move[MAX_COUNT_TURNS][2], int* n, COORD path[MAX_COUNT_TURNS][8])
{
	if (depth == 0) return evaluate(turn);
	int score = -10000, is_cut = 0, was_queen[10], op_turn = 3 - turn;
	COORD moves[MAX_COUNT_TURNS][2], eated_checker[MAX_COUNT_TURNS][9];
	memset(eated_checker, 0, sizeof(eated_checker));
	memset(moves, 0, sizeof(moves));
	memset(was_queen, 0, sizeof(was_queen));
	analyze(TMP);
	find_moves(moves, eated_checker, turn, path);//ïðîñ÷åò âñåõ äîñòóïíûõ õîäîâ
	COORD path1[MAX_COUNT_TURNS][8];
	memset(path1, 0, sizeof(path1));
	memcpy(path1, path, sizeof(path1));
	int i = 0;
	while (!(moves[i][0].X == 0 && moves[i][0].Y == 0) && !(moves[i][1].X == 0 && moves[i][1].Y == 0) && alpha < beta)
	{//ïîêà åñòü äîñòóïíûå õîäû è a>=b(ab-îòñå÷åíèå)
		move_forward(&moves[i][0], &moves[i][1], turn, eated_checker, i, was_queen);
		int tmp = alpha_beta(global_turn, op_turn, depth - 1, alpha, beta, save_move, n, path);
		move_back(&moves[i][0], &moves[i][1], turn, &eated_checker[i], was_queen, ALL);
		was_queen[9] = 0;// [9] ïîêàçûâàåò äàìêà ëè øàøêà, êîòîðàÿ õîäèò
		if (depth == HARD_DEPTH && tmp > alpha)// åñëè íàõîäèìñÿ íà âåðøèíå è áîëüøå óæå íàéäåííîãî, çíà÷èò çàïèñûâàåì
		{
			(*n) = 0;
			memset(save_move, 0, sizeof(COORD) * 56);
			save_move[*n][0] = moves[i][0];
			save_move[*n][1] = moves[i][1];
			(*n)++;
			alpha = tmp;
		}
		else if (depth == HARD_DEPTH && tmp == alpha)// åñëè íàõîäèìñÿ íà âåðøèíå è ðàâíî íàèáîëüøåìó, äîïèñûâàåì â ìàññèâ èäåàëüíûõ õîäîâ
		{
			save_move[*n][0] = moves[i][0];
			save_move[*n][1] = moves[i][1];
			(*n)++;
		}
		else if (depth < HARD_DEPTH && turn == red && tmp > alpha)// åñëè ëó÷øå, ÷åì óæå íàéäåííûé äëÿ êðàñíûõ, çàïîìèíàåì
			alpha = tmp;
		else if (depth < HARD_DEPTH && turn == white && tmp < beta)// åñëè õóæå óæå íàéäåííûé äëÿ áåëûõ, çàïîìèíàåì
			beta = tmp;
		i++;
	}
	if (depth == HARD_DEPTH)//îêîí÷àòåëüíàÿ îáðàáîòêà ðåçóëüòàòà
	{
		memcpy(path, path1, sizeof(path1));
		int act;
		int g = 1;
		while (!(save_move[g][0].X == 0 && save_move[g][0].Y == 0))
			g++;//ñ÷èòàåì êîëè÷åñòâî ëó÷øèõ õîäîâ
		srand(time(NULL));
		act = rand() % g;// åñëè èõ íåñêîëüêî, âûáèðàåì ðàíäîìîì
		g = 0;
		while (!(save_move[act][0].X == moves[g][0].X && save_move[act][0].Y == moves[g][0].Y && save_move[act][1].X == moves[g][1].X && save_move[act][1].Y == moves[g][1].Y))
			g++;// èùåì èíäåêñ èäåàëüíîãî õîäà â ìàññèâå õîäîâ
		(*n) = act;
		return g;//âîçâðàùàåì òîò ñàìûé èíäåêñ
	}
	if (moves[0][0].X == 0 && moves[0][0].Y == 0 && moves[0][1].X == 0 && moves[0][1].Y == 0 && turn != global_turn)
		return 200 + depth;//åñëè õîäîâ áåëûõ íåò, çíà÷èò ïîáåäà äëÿ êðàñíûõ è äàåì î÷åíü áîëüøîå êîë-âî î÷êîâ
	if (moves[0][0].X == 0 && moves[0][0].Y == 0 && moves[0][1].X == 0 && moves[0][1].Y == 0 && turn == global_turn)
		return -200 + depth;//åñëè õîäîâ êðàñíûõ íåò, çíà÷èò ïîáåäà äëÿ áåëûõ è äàåì î÷åíü ìàëåíüêîå êîë-âî î÷êîâ
	if (alpha >= beta && turn == global_turn)
		return 10000;//Åñëè íàðóøèëîñü óñëîâèå ab => íå ñ÷èòàåì ýòó âåòêó çà èäåàëüíóþ
	if (alpha >= beta && turn != global_turn)
		return -10000;//Åñëè íàðóøèëîñü óñëîâèå ab => íå ñ÷èòàåì ýòó âåòêó çà èäåàëüíóþ
	if (turn == global_turn)
		return alpha;//Ïîëó÷èâøååñÿ çíà÷åíèå ïåðåäàåì âî âíåøíþþ ôóíêöèþ äëÿ ñðàâíåíèÿ ñ îäíîóðîâíåâûìè
	if (turn != global_turn)
		return beta;//Ïîëó÷èâøååñÿ çíà÷åíèå ïåðåäàåì âî âíåøíþþ ôóíêöèþ äëÿ ñðàâíåíèÿ ñ îäíîóðîâíåâûìè
}

void find_moves(COORD moves[MAX_COUNT_TURNS][2], COORD eated_checker[MAX_COUNT_TURNS][9], int turn, COORD path[MAX_COUNT_TURNS][8])
{
	COORD crd = { 0 , 0 }, eated_checker_local = { 0 , 0 }, crd_old = { 0 , 0 }, crd_new = { 0 , 0 };
	int ind = 0, was_queen = 0;
	int cut = have_to_cut(turn, 2), result;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			//int i_cur = i, j_cur = j;
			crd.X = i;
			crd.Y = j;
			if (cut == 1 && field_tmp[i][j].isQueen == false && field_tmp[i][j].isBusy == turn)
				deep_search(turn, crd, moves, eated_checker, &ind, crd, 0, path);
			else if (cut == 1 && field_tmp[i][j].isBusy == turn)
				deep_search_for_queen(turn, crd, moves, eated_checker, &ind, crd, 0, path);
			else if (field_tmp[i][j].isQueen == 0 && field_tmp[i][j].isBusy == turn)
			{
				for (int n = 0; n < 4; n++)
					if (!(field_tmp[i][j].step[n].X == 0 && field_tmp[i][j].step[n].Y == 0))
					{
						moves[ind][0].X = i;
						moves[ind][0].Y = j;
						moves[ind][1].X = field_tmp[i][j].step[n].X;
						moves[ind][1].Y = field_tmp[i][j].step[n].Y;
						ind++;
					}
			}
			else if (field_tmp[i][j].isBusy == turn)
			{
				for (int n = 0; n < 4; n++)
					for (int k = 0; k < 7; k++)
						if (!(field_tmp[i][j].step_q[n][k].X == 0 && field_tmp[i][j].step_q[n][k].Y == 0))
						{
							moves[ind][0].X = i;
							moves[ind][0].Y = j;
							moves[ind][1].X = field_tmp[i][j].step_q[n][k].X;
							moves[ind][1].Y = field_tmp[i][j].step_q[n][k].Y;
							ind++;
						}
			}
		}
	analyze(TMP);
}

void move_forward(COORD* old_pos, COORD* new_pos, int turn, COORD eated_checker[MAX_COUNT_TURNS][9], int ind, int was_queen[10])
{
	COORD tmp = { 0,0 };
	field_tmp[new_pos->X][new_pos->Y].isBusy = field_tmp[old_pos->X][old_pos->Y].isBusy;
	field_tmp[old_pos->X][old_pos->Y].isBusy = no;
	field_tmp[new_pos->X][new_pos->Y].isQueen = field_tmp[old_pos->X][old_pos->Y].isQueen;
	if (field_tmp[old_pos->X][old_pos->Y].isQueen == true)
		was_queen[9] = 1;
	field_tmp[old_pos->X][old_pos->Y].isQueen = false;
#ifdef SHOW_TURN_AI
	con_set_color(CON_CLR_BLACK, CON_CLR_BLACK);
	print_cell(field_tmp[new_pos->X][new_pos->Y].crd);
	print_cell(field_tmp[old_pos->X][old_pos->Y].crd);
#endif
	if (turn == red)
		con_set_color(CON_CLR_RED, CON_CLR_BLACK);
	else
		con_set_color(CON_CLR_WHITE, CON_CLR_BLACK);
	if (turn == white && new_pos->X == 0 || turn == red && new_pos->X == 7 || field_tmp[old_pos->X][old_pos->Y].isQueen == true)
	{
		field_tmp[old_pos->X][old_pos->Y].isQueen = false;
		field_tmp[new_pos->X][new_pos->Y].isQueen = true;
	}
#ifdef SHOW_TURN_AI
	if (field_tmp[new_pos->X][new_pos->Y].isQueen == false)
		print_model(field_tmp[new_pos->X][new_pos->Y].crd);
	else
		print_queen(field_tmp[new_pos->X][new_pos->Y].crd);
#endif	
	int i = 0;
	while (!(eated_checker[ind][i].X == 0 && eated_checker[ind][i].Y == 0))
	{
		eating_theory(eated_checker[ind][i]);
		i++;
	}
}

void move_back(COORD* old_pos, COORD* new_pos, int turn, COORD eated_checker[9], int was_queen[10], int count)
{
	field_tmp[old_pos->X][old_pos->Y].isBusy = field_tmp[new_pos->X][new_pos->Y].isBusy;
	if (was_queen[9] == 1)
		field_tmp[old_pos->X][old_pos->Y].isQueen = true;
	else
		field_tmp[old_pos->X][old_pos->Y].isQueen = false;
	field_tmp[new_pos->X][new_pos->Y].isBusy = no;
	field_tmp[new_pos->X][new_pos->Y].isQueen = false;
	if (eated_checker[0].X != 0)
	{
		int i = 0;
		if (count == ALL)
		{
			while (!(eated_checker[i].X == 0 && eated_checker[i].Y == 0) && i < 9)
				field_tmp[eated_checker[i].X][eated_checker[i++].Y].isBusy = 3 - turn;
		}
		else
		{
			i = 8;
			while (eated_checker[i].X == 0 && eated_checker[i].Y == 0 && i > -1)
				i--;
			field_tmp[eated_checker[i].X][eated_checker[i].Y].isBusy = 3 - turn;
		}
		if (turn == red)
			con_set_color(CON_CLR_WHITE, CON_CLR_BLACK);
		else
			con_set_color(CON_CLR_RED, CON_CLR_BLACK);
		if (count == ALL)
		{
			for (i = 0; i < 9; i++)
			{
				if (!(eated_checker[i].X == 0 && eated_checker[i].Y == 0) && was_queen[i] != 0)
					field_tmp[eated_checker[i].X][eated_checker[i].Y].isQueen = true;
				if (!(eated_checker[i].X == 0 && eated_checker[i].Y == 0) && was_queen[i] == 0)
					field_tmp[eated_checker[i].X][eated_checker[i].Y].isQueen = false;
			}	
#ifdef SHOW_TURN_AI
			for (i = 0; i < 9; i++)
			{
				if (!(eated_checker[i].X == 0 && eated_checker[i].Y == 0) && was_queen[i] != 0)
					print_queen(field_tmp[eated_checker[i].X][eated_checker[i].Y].crd);
				if (!(eated_checker[i].X == 0 && eated_checker[i].Y == 0) && was_queen[i] == 0)
					print_model(field_tmp[eated_checker[i].X][eated_checker[i].Y].crd);
			}
#endif
		}
		else
		{
			i = 8;
			while (eated_checker[i].X == 0 && eated_checker[i].Y == 0 && i > -1)
				i--;
			if (i > -1 && was_queen[i] == 0)
				field_tmp[eated_checker[i].X][eated_checker[i].Y].isQueen = false;
			else if (i > -1 && was_queen[i] == 1)
				field_tmp[eated_checker[i].X][eated_checker[i].Y].isQueen = true;
#ifdef SHOW_TURN_AI
			i = 8;
			while (eated_checker[i].X == 0 && eated_checker[i].Y == 0 && i > -1)
				i--;
			if (i > -1 && was_queen[i] == 0)
				print_model(field_tmp[eated_checker[i].X][eated_checker[i++].Y].crd);
			else if (i > -1 && was_queen[i] == 1)
				print_queen(field_tmp[eated_checker[i].X][eated_checker[i++].Y].crd);
#endif
		}
	}
#ifdef SHOW_TURN_AI
	con_set_color(CON_CLR_BLACK, CON_CLR_BLACK);

	print_cell(field_tmp[new_pos->X][new_pos->Y].crd);
	print_cell(field_tmp[old_pos->X][old_pos->Y].crd);
	if (turn == red)
		con_set_color(CON_CLR_RED, CON_CLR_BLACK);
	else
		con_set_color(CON_CLR_WHITE, CON_CLR_BLACK);
	if (field_tmp[old_pos->X][old_pos->Y].isQueen == false)
		print_model(field_tmp[old_pos->X][old_pos->Y].crd);
	else
		print_queen(field_tmp[old_pos->X][old_pos->Y].crd);
#endif
}

int evaluate(int turn)
{
	int value = 0, inc, n, k;
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (field_tmp[i][j].isBusy == 3 - turn)// øòðàô çà íàëè÷èå âðàæåñêèõ øàøåê
			{
				if (3 - turn == white)
					value -= 18 - i;
				else
					value -= i + 11;
			}
			if (field_tmp[i][j].isBusy == turn)
			{
				if (turn == red)//âàæíàÿ ïîçèöèÿ
				{
					if (i == 4 && j == 7)
						value += 5;
					if (field_tmp[3][0].isBusy == white)
						value -= 5;
				}
				if (turn == white)//âàæíàÿ ïîçèöèÿ
				{
					if (i == 3 && j == 0)
						value += 5;
					if (field_tmp[4][7].isBusy == red)
						value -= 5;
				}
				value += is_weak(i, j);//ñëàáûå ìåñòà êîíêðåòíîé øàøêè
				value += help_defend(i, j);//ïîìîãàåò ëè çàùèùàòü ñîñåäíèå
				if (field_tmp[i][j].isQueen == true)// ïðîâåðêà, ðóáèò ëè ÷óæèå øàøêè
				{
					value += 8;
					n = i;
					k = j;
					inc = 3;
					while (--n > 0 && --k > 0)
						if (field_tmp[n][k].isBusy == 3 - turn && field_tmp[n - 1][k - 1].isBusy == no)
							value += inc;
					n = i;
					k = j;
					while (--n > 0 && ++k < 7)
						if (field_tmp[n][k].isBusy == 3 - turn && field_tmp[n - 1][k + 1].isBusy == no)
							value += inc;
					n = i;
					k = j;
					while (++n < 7 && --k > 0)
						if (field_tmp[n][k].isBusy == 3 - turn && field_tmp[n + 1][k - 1].isBusy == no)
							value += inc;
					n = i;
					k = j;
					while (++n < 7 && ++k < 7)
						if (field_tmp[n][k].isBusy == 3 - turn && field_tmp[n + 1][k + 1].isBusy == no)
							value += inc;
				}
				else if (turn == red)// áîíóñ çà ïðîäâèæåíèå ê äàìêå
					value += i + 11;
				else if (turn == white)// áîíóñ çà ïðîäâèæåíèå ê äàìêå
					value += 18 - i;
				}
			}
	return value;
		}

void eating_theory(COORD crd)//ðóáêà â ïðîñ÷åòå õîäîâ
{
	field_tmp[crd.X][crd.Y].isBusy = no;
	field_tmp[crd.X][crd.Y].isQueen = false;
#ifdef SHOW_TURN_AI
	print_cell(field_tmp[crd.X][crd.Y].crd);
#endif
}

int is_weak(int X, int Y)
{
	int cord_x, cord_y;
	int turn = field_tmp[X][Y].isBusy;
	int power = 0;
	if (X > 0 && Y > 0 && X < 7 && Y < 7 && field_tmp[X + 1][Y + 1].isBusy == no && field_tmp[X - 1][Y - 1].isBusy == 3 - turn)
		power -= 5;//íàõîäèòñÿ ëè ïîä ðóáêîé øàøêè(íèæå àíàëîãè÷íî)
	if (X > 0 && Y > 0 && X < 7 && Y < 7 && field_tmp[X - 1][Y - 1].isBusy == no && field_tmp[X + 1][Y + 1].isBusy == 3 - turn)
		power -= 5;
	if (X > 0 && Y > 0 && X < 7 && Y < 7 && field_tmp[X - 1][Y + 1].isBusy == no && field_tmp[X + 1][Y - 1].isBusy == 3 - turn)
		power -= 5;
	if (X > 0 && Y > 0 && X < 7 && Y < 7 && field_tmp[X + 1][Y - 1].isBusy == no && field_tmp[X - 1][Y + 1].isBusy == 3 - turn)
		power -= 5;
	if (X > 1 && Y > 1 && X < 7 && Y < 7 && field_tmp[X - 1][Y - 1].isBusy == 3 - turn && field_tmp[X - 2][Y - 2].isBusy == no)
		power += 5;
	if (X > 1 && Y > 0 && X < 7 && Y < 6 && field_tmp[X - 1][Y + 1].isBusy == 3 - turn && field_tmp[X - 2][Y + 2].isBusy == no)
		power += 5;
	if (X > 0 && Y > 0 && X < 6 && Y < 6 && field_tmp[X + 1][Y + 1].isBusy == 3 - turn && field_tmp[X + 2][Y + 2].isBusy == no)
		power += 5;
	if (X > 0 && Y > 1 && X < 6 && Y < 7 && field_tmp[X + 1][Y - 1].isBusy == 3 - turn && field_tmp[X + 2][Y - 2].isBusy == no)
		power += 5;
	if (X < 7 && X > 0 && Y > 0 && Y < 7) // íàõîäèòñÿ ëè ïîä ðóáêîé äàìêè
	{
		if (field_tmp[X + 1][Y + 1].isBusy == no)
		{
			cord_x = X;
			cord_y = Y;
			while (cord_x > -1 && cord_y > -1 && !(field_tmp[cord_x][cord_y].isBusy == 3 - turn && field_tmp[cord_x][cord_y].isQueen == true))
			{
				cord_x--;
				cord_y--;
			}
			if (cord_x > -1 && cord_y > -1)
				power -= 7;
		}
		if (field_tmp[X - 1][Y - 1].isBusy == no)
		{
			cord_x = X;
			cord_y = Y;
			while (cord_x < 8 && cord_y < 8 && !(field_tmp[cord_x][cord_y].isBusy == 3 - turn && field_tmp[cord_x][cord_y].isQueen == true))
			{
				cord_x++;
				cord_y++;
			}
			if (cord_x < 8 && cord_y < 8)
				power -= 7;
		}
		if (field_tmp[X + 1][Y - 1].isBusy == no)
		{
			cord_x = X;
			cord_y = Y;
			while (cord_x > -1 && cord_y < 8 && !(field_tmp[cord_x][cord_y].isBusy == 3 - turn && field_tmp[cord_x][cord_y].isQueen == true))
			{
				cord_x--;
				cord_y++;
			}
			if (cord_x > -1 && cord_y < 8)
				power -= 7;
		}
		if (field_tmp[X - 1][Y + 1].isBusy == no)
		{
			cord_x = X;
			cord_y = Y;
			while (cord_x < 8 && cord_y > -1 && !(field_tmp[cord_x][cord_y].isBusy == 3 - turn && field_tmp[cord_x][cord_y].isQueen == true))
			{
				cord_x++;
				cord_y--;
			}
			if (cord_x < 8 && cord_y > -1)
				power -= 7;
		}
	}
	return power;
}

int help_defend(int i, int j)
{
	int turn = field_tmp[i][j].isBusy, value = 0;
	if (i > 1 && j > 1)
		if (field_tmp[i - 1][j - 1].isBusy == turn && field_tmp[i - 2][j - 2].isBusy == 3 - turn)
			value += 7;
	if (i > 1 && j < 6)
		if (field_tmp[i - 1][j + 1].isBusy == turn && field_tmp[i - 2][j + 2].isBusy == 3 - turn)
			value += 7;
	if (i < 6 && j > 1)
		if (field_tmp[i + 1][j - 1].isBusy == turn && field_tmp[i + 2][j - 2].isBusy == 3 - turn)
			value += 7;
	if (i < 6 && j < 6)
		if (field_tmp[i + 1][j + 1].isBusy == turn && field_tmp[i + 2][j + 2].isBusy == 3 - turn)
			value += 7;
	return value;
}

void deep_search(int turn, COORD crd, COORD moves[MAX_COUNT_TURNS][2], COORD eated_checker[MAX_COUNT_TURNS][9], int* ind, COORD start_coord, int i, COORD path[MAX_COUNT_TURNS][8])
{
	COORD new_coord = { 0,0 };
	int result = 0, is_cut = 0, was_queen[10], k = 0, first = 1;
	memset(was_queen, 0, sizeof(was_queen));
	int ind_for_copy = (*ind) - 1;// èíäåêñ äëÿ êîïèðîâàíèÿ ïðåäûäóùèõ ÷àñòåé õîäà
	int count_eated_prev = i;//êîëè÷åñòâî ñúåäåíûõ øàøåê äî ýòîãî õîäà
	for (int n = 0; n < 4; n++)//öèêë ïî âñåì ðóáêàì
	{
		analyze(TMP);
		if (!(field_tmp[crd.X][crd.Y].cut[n].X == 0 && field_tmp[crd.X][crd.Y].cut[n].Y == 0))
		{//åñëè åñòü äîñòóïíàÿ ðóáêà
			moves[*ind][0].X = start_coord.X;//çàïèñûâàåì êàê õîä
			moves[*ind][0].Y = start_coord.Y;
			moves[*ind][1].X = field_tmp[crd.X][crd.Y].cut[n].X;
			moves[*ind][1].Y = field_tmp[crd.X][crd.Y].cut[n].Y;
			int z = 0;
			while (!(eated_checker[ind_for_copy][z].X == 0 && eated_checker[ind_for_copy][z].Y == 0) && ind_for_copy > -1)
			{//åñëè äî ýòîãî â ñåðèè õîäîâ áûëè ñúåäåíû øàøêè, êîïèðóåì èõ äëÿ òåêóùåãî õîäà
				eated_checker[*ind][z].X = eated_checker[ind_for_copy][z].X;
				eated_checker[*ind][z].Y = eated_checker[ind_for_copy][z].Y;
				z++;
			}
			if (start_coord.X == crd.X && start_coord.Y == crd.Y && first == 1)
			{//äëÿ ïåðâîãî âõîäà, îáíóëÿåì ìàññèâ
				memset(eated_checker, 0, sizeof(eated_checker));
				i = 0;
				first = 0;
			}
			else if (start_coord.X == crd.X && start_coord.Y == crd.Y)
				i = 0;// åñëè ïåðâûé õîä èìåë íåñêîëüêî âàðèàöèé, ñúåäåíûõ øàøåê äëÿ âòîðîé è òä âàðèàöèé áóäåò 0
			else
			{
				int k = 0;
				while (!(path[ind_for_copy][k].X == 0 && path[ind_for_copy][k].Y == 0))
				{//åñëè ïóòü ñîñòîèò íå èç îäíîé ðóáêè, çàïèñûâàåì ïðîìåæóòî÷íûå ïîçèöèè
					path[*ind][k].X = path[ind_for_copy][k].X;
					path[*ind][k].Y = path[ind_for_copy][k].Y;
					k++;
				}
				path[*ind][k].X = crd.X;//çàïèñûâàåì òåêóùóþ ïîçèöèþ, êàê ÷àñòü ïóòè
				path[*ind][k].Y = crd.Y;
			}
			eated_checker[*ind][count_eated_prev] = find_eated(turn, crd, field_tmp[crd.X][crd.Y].cut[n]);//øàøêà, ñúåäåííàÿ ýòèì õîäîì
			move_forward(&crd, &field_tmp[crd.X][crd.Y].cut[n], turn, eated_checker, *ind, &was_queen);
			int ind_for_del = *ind;
			(*ind)++;
			i++;
			new_coord.X = field_tmp[crd.X][crd.Y].cut[n].X;//íîâîé ïîçèöèåé áóäåò ðåçóëüòàò ðóáêè
			new_coord.Y = field_tmp[crd.X][crd.Y].cut[n].Y;
			analyze(TMP);
			result = have_to_cut_cur(new_coord, 2);// ïðîâåðÿåì, ìîæåò ëè ñðóáèòü åù¸
			if (result == 1)// åñëè ìîæåò ñðóáèòü åùå îäíó, âõîäèì â ðåêóðñèþ
				deep_search(turn, new_coord, moves, eated_checker, ind, start_coord, i, path);
			i--;
			move_back(&crd, &new_coord, turn, &eated_checker[ind_for_del], was_queen, ONE);
		}
	}
}

void deep_search_for_queen(int turn, COORD crd, COORD moves[MAX_COUNT_TURNS][2], COORD eated_checker[MAX_COUNT_TURNS][9], int* ind, COORD start_coord, int i, COORD path[MAX_COUNT_TURNS][8])
{
	COORD new_coord = { 0,0 };
	int result = 0, is_cut = 0, was_queen[10], k = 0, first = 1;
	memset(was_queen, 0, sizeof(was_queen));
	int ind_for_copy = (*ind) - 1;// èíäåêñ äëÿ êîïèðîâàíèÿ ïðåäûäóùèõ ÷àñòåé õîäà
	int count_eated_prev = i;//êîëè÷åñòâî ñúåäåíûõ øàøåê äî ýòîãî õîäà
	for (int n = 0; n < 4; n++)
	{
		for (int t = 0; t < 6; t++)//öèêë ïî âñåì ðóáêàì
		{
			analyze(TMP);
			if (!(field_tmp[crd.X][crd.Y].cut_q[n][t].X == 0 && field_tmp[crd.X][crd.Y].cut_q[n][t].Y == 0))
			{//åñëè åñòü äîñòóïíàÿ ðóáêà
				moves[*ind][0].X = start_coord.X;//çàïèñûâàåì êàê õîä
				moves[*ind][0].Y = start_coord.Y;
				moves[*ind][1].X = field_tmp[crd.X][crd.Y].cut_q[n][t].X;
				moves[*ind][1].Y = field_tmp[crd.X][crd.Y].cut_q[n][t].Y;
				int z = 0;
				while (!(eated_checker[ind_for_copy][z].X == 0 && eated_checker[ind_for_copy][z].Y == 0) && ind_for_copy > -1)
				{//åñëè äî ýòîãî â ñåðèè õîäîâ áûëè ñúåäåíû øàøêè, êîïèðóåì èõ äëÿ òåêóùåãî õîäà
					eated_checker[*ind][z].X = eated_checker[ind_for_copy][z].X;
					eated_checker[*ind][z].Y = eated_checker[ind_for_copy][z].Y;
					z++;
				}
				if (start_coord.X == crd.X && start_coord.Y == crd.Y && first == 1)
				{//äëÿ ïåðâîãî âõîäà, îáíóëÿåì ìàññèâ
					memset(eated_checker, 0, sizeof(eated_checker));
					i = 0;
					first = 0;
				}
				else if (start_coord.X == crd.X && start_coord.Y == crd.Y)
					i = 0;// åñëè ïåðâûé õîä èìåë íåñêîëüêî âàðèàöèé, ñúåäåíûõ øàøåê äëÿ âòîðîé è òä âàðèàöèé áóäåò 0
				else
				{
					int k = 0;
					while (!(path[ind_for_copy][k].X == 0 && path[ind_for_copy][k].Y == 0))
					{//åñëè ïóòü ñîñòîèò íå èç îäíîé ðóáêè, çàïèñûâàåì ïðîìåæóòî÷íûå ïîçèöèè
						path[*ind][k].X = path[ind_for_copy][k].X;
						path[*ind][k].Y = path[ind_for_copy][k].Y;
						k++;
					}
					path[*ind][k].X = crd.X;//çàïèñûâàåì òåêóùóþ ïîçèöèþ, êàê ÷àñòü ïóòè
					path[*ind][k].Y = crd.Y;
				}
				eated_checker[*ind][i] = find_eated(turn, crd, field_tmp[crd.X][crd.Y].cut_q[n][t]);//øàøêà, ñúåäåííàÿ ýòèì õîäîì
				move_forward(&crd, &field_tmp[crd.X][crd.Y].cut_q[n][t], turn, eated_checker, *ind, &was_queen);
				int ind_for_del = *ind;
				(*ind)++;
				i++;
				new_coord.X = field_tmp[crd.X][crd.Y].cut_q[n][t].X;//íîâîé ïîçèöèåé áóäåò ðåçóëüòàò ðóáêè
				new_coord.Y = field_tmp[crd.X][crd.Y].cut_q[n][t].Y;
				analyze(TMP);
				result = have_to_cut_cur(new_coord, 2);// ïðîâåðÿåì, ìîæåò ëè ñðóáèòü åù¸
				if (result == 1)// åñëè ìîæåò ñðóáèòü åùå îäíó, âõîäèì â ðåêóðñèþ
					deep_search_for_queen(turn, new_coord, moves, eated_checker, ind, start_coord, i, path);
				i--;
				move_back(&crd, &new_coord, turn, &eated_checker[ind_for_del], was_queen, ONE);
			}
		}
	}
}

COORD find_eated(int turn, COORD start, COORD finish)
{// íàõîäèì øàøêó, êîòîðóþ ñúåëè ïóòåì ïðèáàâëåíèÿ/îòíÿòèÿ ñîîòâ. êîîðäèíàò
	int incx = start.X - finish.X > 0 ? -1 : 1;
	int incy = start.Y - finish.Y > 0 ? -1 : 1;
	while (field_tmp[start.X][start.Y].isBusy != 3 - turn)
	{
		start.X += incx;
		start.Y += incy;
	}
	return start;
}
