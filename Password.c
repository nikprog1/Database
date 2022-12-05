#include "main.h"
GtkEntry *gPassword; // Буфер пароля
GtkEntry *gLogin; // Буфер Логина
enum
{
    C_ID_SOTR,C_LOGIN,C_PAROL
};

char Password[100];
char Login[100];
char Slogin[100];
char Spassword[100];
char SQL[100];

int comparision()
{
    comin=0;
    // Получение из окна  Пароля и Логина
    sprintf(Password, "%s", gtk_entry_get_text(gPassword));
	sprintf(Login, "%s", gtk_entry_get_text(gLogin));
    // Проверка на пустые поля
    if(strlen(Login)!=0 && strlen(Password)!=0)
    {
        sprintf(Login, "%s", gtk_entry_get_text(gLogin));
        lenstr = 0;
        lenstr+=sprintf(SQL,"SELECT [Код сотрудника],Логин,Пароль FROM Сотрудники WHERE Логин = ");
        lenstr+=sprintf(SQL+lenstr, "'%s'",Login);
    }
    else
    {
        ShowInfo();
        return 0;
    }

// Готовим SQL-запрос к БД
if(sqlite3_prepare_v2(db, SQL, -1, &stmt, NULL) != SQLITE_OK)
{
        sprintf(str, "Ошибка подготовки SQL-запроса: %s\n", sqlite3_errmsg(db));
		ShowError();
        sqlite3_finalize(stmt);
        return 3;
}
// Выполнение SQL-запроса и проверка на совпадение логина и пароля
    rc = sqlite3_step(stmt);
    if (rc==100)
    {
    sprintf(Slogin,"%s",sqlite3_column_text(stmt, C_LOGIN));
    sprintf(Spassword,"%s",sqlite3_column_text(stmt, C_PAROL));
    if (strcmp(Login,Slogin)==0 && (strcmp(Password,Spassword)==0))
    {
    comin=1;
    gtk_widget_destroy(wPassword);
    ShowMainWindow();
    }
    else
    {
    ShowInfo();
    }
    }
     else
    {
    ShowInfo();
    }

// Освобождаем строку запроса
    sqlite3_finalize(stmt);
    return 0;
}

G_MODULE_EXPORT void on_button11_clicked (GtkButton *button, gpointer label)
{
    comparision();
}

G_MODULE_EXPORT void on_wPassword_destroy (GtkContainer *widget, gpointer label)
{
if (comin==0)
{
gtk_main_quit();
}
}



// Вывод пароля
int ShowPassword()
{
    bPassword = gtk_builder_new ();
    gtk_builder_add_from_file (bPassword, "main.glade", NULL);
    wPassword = GTK_WIDGET(gtk_builder_get_object(bPassword, "wPassword"));
    gPassword = GTK_ENTRY(gtk_builder_get_object(bPassword, "entry_Password"));
	gLogin = GTK_ENTRY(gtk_builder_get_object(bPassword, "entry_Login"));
    gtk_window_set_default_size (GTK_WINDOW (wPassword), 400, 300);
    gtk_window_set_position(GTK_WINDOW(wPassword), GTK_WIN_POS_CENTER);
    gtk_window_set_modal(GTK_WINDOW(wPassword),TRUE);
    gtk_builder_connect_signals (bPassword, NULL);
    //g_signal_connect(G_OBJECT(wPassword), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(wPassword);
    return 1;
}
