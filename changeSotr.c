#include "main.h"
GtkEntry *gFamSotr; // Название объекта
GtkEntry *gNameSotr; // Географическое расположение объектов
GtkComboBoxText *gDoljnSotr; // Буквенный индекс системы
GtkComboBoxText *gStatusSotr; // Буквенный индекс системы

int Sotr;
int Status;
char sotrfamil[1000];//Фамилия сотрудника
char sotrdoljn[1000];//Должность сотрудника
int Sotrudnichestvo;//Статус сотрудничества

// Обозначения полей таблицы "Сотрудники"
enum
{
    C_ID_SOTR,C_FAMIL_SOTR,C_NAME_SOTR,C_DOLJN_SOTR, C_KOD_SOTRUDNICHESTVA_SOTR
};


int LoadSotrDanniy(int Sotr)
{
    //ID_proj=proj;
    // Запись в цифровые виджеты
    char* SQL = "SELECT [Код сотрудника],Фамилия,Имя,Должность,Сотрудничество FROM Сотрудники\
    WHERE [Код сотрудника]=";
    sprintf(str, "%s%d",SQL, Sotr);
    //ShowSQL(); // Тестовый вывод запроса
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK )
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Выполнение SQL-запроса
    rc = sqlite3_step(stmt);
    //Получение из базы и запись в виджет данных фамилии сотрудника
    sprintf(sotrfamil,"%s", sqlite3_column_text(stmt, C_FAMIL_SOTR));
    gtk_entry_set_text(GTK_ENTRY(gFamSotr),sotrfamil);

    /*   //Получение из базы и запись в виджет данных имени сотрудника
        char sotrname[1000];
        sprintf(sotrname,"%s", sqlite3_column_text(stmt, C_NAME_SOTR));
        gtk_entry_set_text(GTK_ENTRY(gNameSotr),sotrname);*/
    //Получение из базы и запись в виджет данных должности сотрудника
    sprintf(sotrdoljn,"%s", sqlite3_column_text(stmt, C_DOLJN_SOTR));
    //Получение из базы данных о статусе сотрудничества сотрудника
    Sotrudnichestvo=sqlite3_column_int(stmt, C_KOD_SOTRUDNICHESTVA_SOTR);
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    SpisDoljn();
    SpisSotrudnichestvo(Sotr);
    return 0;
}

int SpisDoljn() // ЗАПОЛНЕНИЕ ВЫПАДАЮЩЕГО СПИСКА "ДОЛЖНОСТЬ"
{
    char buf[100];
    char* SQL1 = "SELECT DISTINCT Должность FROM Сотрудники";
    sprintf(str, "%s",SQL1);
    //ShowSQL(); // Тестовый вывод запроса
    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, SQL1, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Цикл по SQL-запросу и запись в выпадающий список "Менеджеры"
    int sysindex=-1;
    int sysindex1=-1;
    // int kodsotrudnichestva;
    while((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        sysindex++;
        sprintf(buf,"%s", sqlite3_column_text(stmt,0));
        if(strcmp(buf,sotrdoljn)==0)
        {
            // Установка активного элемента
            sysindex1=sysindex;
            gtk_combo_box_text_append_text(gDoljnSotr, buf);
            gtk_combo_box_set_active(GTK_COMBO_BOX(gDoljnSotr),sysindex1);
            continue;
        }
        gtk_combo_box_text_append_text(gDoljnSotr, buf);

    }

    // Если ошибка
    if(rc != SQLITE_DONE)
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 4;
    }
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// ЗАПОЛНЕНИЕ ВЫПАДАЮЩЕГО СПИСКА "СОТРУДНИЧЕСТВО"
int SpisSotrudnichestvo(int Sotr)
{
    gtk_combo_box_text_append_text(gStatusSotr, "Нет");
    gtk_combo_box_text_append_text(gStatusSotr, "Да");

    char* SQL = "SELECT Сотрудничество FROM Сотрудники\
    WHERE [Код сотрудника]=";
    sprintf(str, "%s%d",SQL,Sotr);
    //ShowSQL(); // Тестовый вывод запроса
    // Готовим SQL-запрос к БД
    if(sqlite3_prepare_v2(db, str, -1, &stmt, NULL) != SQLITE_OK)
    {
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
        ShowError();
        sqlite3_finalize(stmt);
        return 3;
    }
    // Выполнение SQL-запроса
    rc = sqlite3_step(stmt);
    if (sqlite3_column_int(stmt,0)==1)
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(gStatusSotr),1);
    }
    else
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(gStatusSotr),0);
    }
    // Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

// Сохранение записи
void SaveChangeSotr(int sotr)
{
    // Получение из combo_box должности сотрудника
    sprintf(sotrdoljn, "'%s'", gtk_combo_box_text_get_active_text(gDoljnSotr));
    // Получение из combo_box статуса сотрудничества
    char SotrudnichestvoTex[1000]="Да";
    if (strcmp(SotrudnichestvoTex,gtk_combo_box_text_get_active_text(gStatusSotr))==0)
    {
        Sotrudnichestvo=1;
    }
    else
    {
        Sotrudnichestvo=0;
    }

    // Получение фамилии сотрудника
    sprintf(sotrfamil, "'%s'", gtk_entry_get_text(gFamSotr));
    lenstr = 0;
    lenstr+=sprintf(str, "UPDATE Сотрудники SET Фамилия=%s,", sotrfamil);
    lenstr+=sprintf(str+lenstr, "Должность=%s,Сотрудничество=%d",sotrdoljn,Sotrudnichestvo);
    lenstr+=sprintf(str+lenstr, " WHERE [Код сотрудника]=%d", sotr);
    //ShowSQL(); // Тестовый вывод запроса
    // Записываем полученные данные в БД
    if (sqlite3_exec(db, str, 0, 0, &err))
    {
        sprintf(str, "Ошибка выполнения SQL-запроса: %sn", err);
        ShowError();
        sqlite3_free(err);
        sqlite3_finalize(stmt);
        return;
    }
}


// Кнопка Закрыть
G_MODULE_EXPORT void on_button9_clicked (GtkButton *button, gpointer label)
{
    SaveChangeSotr(Sotr);
    //ReloadSotr(Sotr);
    //LoadSotr();
    StatusSotr(Status);
    gtk_widget_destroy(wChangeSotr);
}

void ShowChangeSotr(int ID_Sotr, int StatusSotr)
{
    Status=StatusSotr;
    Sotr=ID_Sotr;
    // Создание билдера
    bChangeSotr = gtk_builder_new ();
    gtk_builder_add_from_file (bChangeSotr, "main.glade", NULL);
    // Создание Виджетов
    wChangeSotr = GTK_WIDGET(gtk_builder_get_object(bChangeSotr, "wChangeSotr"));
    gFamSotr = GTK_ENTRY(gtk_builder_get_object(bChangeSotr, "gFamSotr"));
    gNameSotr = GTK_ENTRY(gtk_builder_get_object(bChangeSotr, "gNameSotr"));
    gDoljnSotr = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bChangeSotr, "gDoljnSotr"));
    gStatusSotr = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(bChangeSotr, "gStatusSotr"));
    gtk_window_set_default_size (GTK_WINDOW (wChangeSotr), 850, 250);
    gtk_window_set_position(GTK_WINDOW(wChangeSotr), GTK_WIN_POS_CENTER);
    gtk_window_set_title (GTK_WINDOW (wChangeSotr), "SotrudnikChanges");// Название окна
    gtk_window_set_icon_from_file (GTK_WINDOW(wChangeSotr), "tables-icon.png", NULL); //Вывод иконки
    gtk_window_set_transient_for(GTK_WINDOW(wChangeSotr),GTK_WINDOW(wSotr));
    gtk_window_set_modal(GTK_WINDOW(wChangeSotr),TRUE);
    gtk_builder_connect_signals (bChangeSotr, NULL);
    if(LoadSotrDanniy(Sotr))
    {
        ShowError();
    }
    gtk_widget_show_all(wChangeSotr);
}
