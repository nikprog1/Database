#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdlib.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <string.h>
#include <math.h>
#include "projects.h"
#include "change.h"
#include "cablechange.h"
#include "Sotrudniki.h"
#include "changeSotr.h"
#include "settings.h"
#include "password.h"

// Структуры данных для базы данных
sqlite3 *db;    // хэндл БД
sqlite3_stmt* stmt; // строка запроса к БД
char *err;
int rc;

// Структуры данных для окон
GtkWidget   *wMain, *wProjects, *wChange,*wCable,*wFindProject, *wSotr, *wChangeSotr, *wPassword, *wLogin;
GtkWidget   *wSettings;
GtkBuilder  *builder1, *builder2, *builder_projects, *bChange, *bCabChange, *builder_FindProject, *bSotr, *bChangeSotr, *bPassword;
GtkBuilder  *bSettings;
GtkCalendar *gCalendar;



char str[1000];
char str2[1000];
int lenstr;
int comin;

void StartDB();
void EndDB();
void ShowError();
int ShowSQL();

// Переменные для отслеживания состояния окон
gboolean activateproj, activateChan, activateFind, activateCable, activateSotr, activateChanSotr;  // Окно проекты

// Виджеты для сохранения итератора неизменным
GtkTreeModel *tm,*tm1; //Модель виджета
GtkTreePath *path,*path1;//Путь к строке


#endif // MAIN_H_INCLUDED
