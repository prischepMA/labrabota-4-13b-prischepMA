#include <stdio.h>
#include <sqlite3.h>
#include<stdlib.h>
#include<string.h>
sqlite3 *db;        // database connection
int rc;             // return code
char *errmsg = 0;       // pointer to an error string

void delChar()
{
    while(getchar()!='\n'){}
}

void PrintMenu()
{
    printf("Welcome to DataBase!\nPlease choose what do you want to do:\n");
    printf("1 - SELECT\n");
    printf("2 - INSERT\n");
    printf("3 - DELETE\n");
    printf("4 - PHOTO TO FILE\n");
    printf("5 - QUIT\n");
    
}

int callback(void *NotUsed, int argc, char **argv,
             char **azColName) {
    NotUsed = 0;
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void selectCountries()
{
    rc = sqlite3_exec(db, "select * from country", callback, 0,&errmsg );
}
void selectCity(char* a)
{
    char query[2000];
    strcpy(query,"select * from city where city.country_id = ");
    strcat(query, a);
    rc = sqlite3_exec(db, query, callback, 0,&errmsg );
}
void selectDepartments()
{
    rc = sqlite3_exec(db, "select * from department", callback, 0,&errmsg );
}

void Select()
{
    printf("Choose which select you want to execute:\n");
    printf("1 - employee's ID\n");
    printf("2 - employee's surname\n");
    printf("3 - country\n");
    printf("4 - all entries on name, surname, department, position, country and city\n");
    printf("5 - to exit\n");char ch;
    char query[2000];
    
    ch = getchar();
    delChar();
    switch (ch)
    {
        case '1':
            printf("Input ID\n");
            char id[15];
            fgets(id,14,stdin);
            delChar();
            strcpy(query,"select employee.first_name, employee.middle_name, employee.last_name, employee.date_of_birth,city.name, country.name, employee.address, department.name, employee.position, employee.employment_date from employee inner join city on city.id = employee.city_id inner join country on country.id = employee.country_id inner join department on department.id = employee.department_id where employee.id = ");
            strcat(query,id);
            rc = sqlite3_exec(db, query, callback, 0,&errmsg);
            if (rc != SQLITE_OK ) {
                
                fprintf(stderr, "Failed to input data\n");
                fprintf(stderr, "SQL error: %s\n", errmsg);
                sqlite3_free(errmsg);
            }
            break;
        case '2':
            printf("Input surname\n");
            char name[30];
            scanf("%s",name);
            delChar();
            strcpy(query,"select employee.first_name, employee.middle_name, employee.last_name, employee.date_of_birth,city.name, country.name, employee.address, department.name, employee.position, employee.employment_date from employee inner join city on city.id = employee.city_id inner join country on country.id = employee.country_id inner join department on department.id = employee.department_id where employee.last_name = '");
            strcat(query,name);
            strcat(query,"'");
            rc = sqlite3_exec(db, query, callback, 0,&errmsg);
            if (rc != SQLITE_OK ) {
                
                fprintf(stderr, "Failed to input data\n");
                fprintf(stderr, "SQL error: %s\n", errmsg);
                sqlite3_free(errmsg);
                
            }
            break;
        case '3':
            printf("Input country\n");
            char country[30];
            scanf("%s",country);
            strcpy(query,"select employee.first_name, employee.middle_name, employee.last_name, employee.date_of_birth,city.name, country.name, employee.address, department.name, employee.position, employee.employment_date from employee inner join city on city.id = employee.city_id inner join country on country.id = employee.country_id inner join department on department.id = employee.department_id where country.name = '");
            strcat(query,country);
            strcat(query,"'");
            
            rc = sqlite3_exec(db, query, callback, 0,&errmsg);
            if (rc != SQLITE_OK ) {
                
                fprintf(stderr, "Failed to input data\n");
                fprintf(stderr, "SQL error: %s\n", errmsg);
                sqlite3_free(errmsg);
            }
            break;
        case '5':
            return;
        case '4':
            strcpy(query,"select employee.first_name, employee.middle_name, employee.last_name, employee.date_of_birth,city.name, country.name, employee.address, department.name, employee.position, employee.employment_date from employee left join city on city.id = employee.city_id left join country on country.id = employee.country_id left join department on department.id = employee.department_id");
            rc = sqlite3_exec(db, query, callback, 0,&errmsg);
            if (rc != SQLITE_OK ) {
                fprintf(stderr, "Failed to input data\n");
                fprintf(stderr, "SQL error: %s\n", errmsg);
                sqlite3_free(errmsg);
            }
            break;
        default:
            printf("Invalid parameter");
            break;
    }
    delChar();
}
void PhotoToFile(){
    sqlite3_stmt *res;
    printf("Input employees ID or 'exit' to exit\n");
    char id[15];
    scanf("%s",id);
    if(memcmp("exit",id,4)==0)
        return;
    delChar();
    char name[100];
    strcpy(name, "/Users/maksim/Documents/employee_photo/employee_");
    strcat(name,id);
    strcat(name,".jpg");
    FILE *fp = fopen(name, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open image file\n");
        return;
    }
    char sql[100] = "SELECT photo FROM employee WHERE id = ";
    strcat(sql,id);
    rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to prepare statement\n");
        sqlite3_close(db);
        return;
    }
    rc = sqlite3_step(res);
    int bytes = 0;
    if (rc == SQLITE_ROW) {
        bytes = sqlite3_column_bytes(res, 0);
    }
    fwrite(sqlite3_column_blob(res, 0), bytes, 1, fp);
    if (ferror(fp)) {
        fprintf(stderr, "fwrite() failed\n");
        return;
    }
    int r = fclose(fp);
    
    if (r == EOF) {
        fprintf(stderr, "Cannot close file handler\n");
    }
    
    rc = sqlite3_finalize(res);
}

void Delete()
{
    printf("Input employee's ID you want to delete or 'exit' to exit:\n");
    char del[15];
    char query[256];
    strcpy(query, "delete from employee where id = ");
    
    fgets(del,14,stdin);
    if(memcmp("exit",del,4)==0)
        return;
    strcat(query,del);
    rc = sqlite3_exec(db, query, 0, 0,&errmsg);
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to delete\n");
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        
    } else
    {
        fprintf(stdout, "Enter was successfully deleted from table 'employee'\n");
    }
}

void Insert()
{
    char name[20];
    char surname[20];
    char patronymic[20];
    char birthdate[40];
    char address[100];
    char photo[100];
    char country [10];
    char city [10];
    char department[10];
    char position [30];
    char start_date[40];
    printf("Input name: ");
    scanf("%s",name);
    
    printf("Input surname: ");
    scanf("%s",surname);
    
    printf("Input middle name: ");
    scanf("%s",patronymic);
    
    printf("Input birthdate: ");
    scanf("%s",birthdate);
    delChar();
    printf("Input address: ");
    fgets(address, 99, stdin);
    printf("Input photo: ");
    scanf("%s",photo);
    delChar();
    printf("Input date of begin working: ");
    fgets(start_date,39,stdin);
    selectDepartments();
    printf("Input department: ");
    fgets(department,14,stdin);
    selectCountries();
    printf("Input country: ");
    fgets(country,15,stdin);
    selectCity(country);
    printf("Input city: ");
    fgets(city,15,stdin);
    printf("Input position: ");
    fgets(position,30,stdin);
    
    char sql_ins [1000]={0};
    strcpy(sql_ins,"insert into employee(first_name,middle_name,last_name,date_of_birth,address, country_id, city_id, department_id, position, employment_date, photo) values('");
    strcat(sql_ins,name);
    strcat(sql_ins,"','");
    strcat(sql_ins,patronymic);
    strcat(sql_ins,"','");
    strcat(sql_ins,surname);
    strcat(sql_ins,"','");
    strcat(sql_ins,birthdate);
    strcat(sql_ins,"','");
    strcat(sql_ins,address);
    strcat(sql_ins,"',");
    strcat(sql_ins, country);
    strcat(sql_ins,",");
    strcat(sql_ins, city);
    strcat(sql_ins,",");
    strcat(sql_ins, department);
    strcat(sql_ins,",'");
    strcat(sql_ins, position);
    strcat(sql_ins,"','");
    strcat(sql_ins,start_date);
    strcat(sql_ins,"',?)");
    
    FILE *fp = fopen(photo, "rb");
    
    if (fp == NULL) {
        
        fprintf(stderr, "Cannot open image file\n");
        
        return;
    }
    
    fseek(fp, 0, SEEK_END);
    
    if (ferror(fp)) {
        
        fprintf(stderr, "fseek() failed\n");
        int r = fclose(fp);
        
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");
        }
        
        return;
    }
    
    int flen = (int)ftell(fp);
    if (flen == -1) {
        perror("error occurred");
        int r = fclose(fp);
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");
        }
        return;
    }
    fseek(fp, 0, SEEK_SET);
    if (ferror(fp)) {
        fprintf(stderr, "fseek() failed\n");
        int r = fclose(fp);
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");
        }
        return;
    }
    char data[flen+1];
    int size = (int)fread(data, 1, flen, fp);
    if (ferror(fp)) {
        fprintf(stderr, "fread() failed\n");
        int r = fclose(fp);
        
        if (r == EOF) {
            fprintf(stderr, "Cannot close file handler\n");
        }
        
        return;
    }
    int r = fclose(fp);
    
    if (r == EOF) {
        fprintf(stderr, "Cannot close file handler\n");
    }
    sqlite3_stmt *pStmt;
    rc = sqlite3_prepare(db, sql_ins, -1, &pStmt, 0);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    
    sqlite3_bind_blob(pStmt, 1, data, size, SQLITE_STATIC);
    rc = sqlite3_step(pStmt);
    if (rc != SQLITE_DONE) {
        printf("execution failed: %s", sqlite3_errmsg(db));
    }
    else
    {
        printf("Input succsed\n");
    }
    
    sqlite3_finalize(pStmt);
}

int main()
{
    rc = sqlite3_open("employees.db", &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(-1);
    }
    system("clear");
    char flag=0;
    while(!flag){
        system("clear");
        PrintMenu();
        char c;
        c=getchar();
        delChar();
        if(c<'1' || c>'5')
        {
            printf("Incorrect input!");
            return 1;
        }
        
        switch(c)
        {
            case '1':
                Select();
                break;
            case'2':
                Insert();
                break;
            case '3':
                Delete();
                break;
            case '5':
                flag = 1;
                break;
            case '4':
                PhotoToFile();
                break;
            default:
                printf("Invalid symbol!");
                break;
        }
        delChar();
    }
    sqlite3_close(db);
    
    return 0;
}
