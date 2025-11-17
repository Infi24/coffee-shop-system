#ifndef USER_H
#define USER_H

#include <stdio.h>

#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 20
#define MAX_PHONE_LEN 15
#define USER_FILE "D:/coffee-shop-system/data/users.txt"  

typedef enum {
    ROLE_CUSTOMER = 0,
    ROLE_MANAGER = 1
} UserRole;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char password[MAX_PASS_LEN];
    char phone[MAX_PHONE_LEN];
    double balance;
    UserRole role;
} User;

// === 公共接口 ===

// 注册（返回新ID，失败返回-1）
int user_register(UserRole role);

// 登录（传入期望角色，返回用户ID，失败返回-1）
int user_login(UserRole role);

// 根据ID查找用户（返回指针或NULL）
User *user_find_by_id(int id);

// 保存用户结构到文件（覆盖式写入）
int user_save(const User *u);

// 从文件加载全部用户到内存（返回数量）
int user_load_all(User *arr, int max);

// 打印单个用户（调试用）
void user_print(const User *u);

#endif
