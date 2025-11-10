#include "user.h"
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 200
static User users[MAX_USERS];        // 用户数据数组，静态存储所有用户信息
static int user_count = 0;           // 当前用户数量
static int loaded = 0;               // 标记是否已从文件加载数据，避免重复加载

/**
 * 内部函数：从数据文件读取所有用户信息到内存数组
 * @param arr 用户数组指针，用于存储读取的用户数据
 * @param max 数组最大容量，防止数组越界
 * @return 成功读取的用户数量，如果文件打开失败返回0
 */
int user_load_all(User *arr, int max) {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return 0;
    int count = 0;
    // 使用fscanf按格式读取每行数据，直到达到数组上限或文件结束
    while (count < max && fscanf(fp, "%d,%49[^,],%19[^,],%14[^,],%lf,%d\n",
            &arr[count].id,           // 用户ID
            arr[count].name,         // 用户名
            arr[count].password,     // 密码
            arr[count].phone,        // 手机号
            &arr[count].balance,     // 余额
            (int *)&arr[count].role) == 6) {  // 角色类型
        count++;
    }
    fclose(fp);
    return count;
}

/**
 * 内部函数：将内存中的所有用户数据写入文件
 * @param arr 用户数组指针，包含要写入的所有用户数据
 * @param count 要写入的用户数量
 * @return 成功写入返回1，失败返回0
 */
static int user_write_all(const User *arr, int count) {
    FILE *fp = fopen(USER_FILE, "w");
    if (!fp) {
        printf("Unable to open user data file for writing!\n");
        return 0;
    }
    // 遍历用户数组，将每个用户的信息按格式写入文件
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d,%s,%s,%s,%.2f,%d\n",
                arr[i].id,           // 用户ID
                arr[i].name,         // 用户名
                arr[i].password,     // 密码
                arr[i].phone,        // 手机号
                arr[i].balance,      // 余额
                arr[i].role);        // 角色
    }
    fclose(fp);
    return 1;
}

/**
 * 初始化加载函数：确保用户数据只从文件加载一次
 * 使用loaded标志位避免程序运行期间重复读取文件
 */
static void ensure_loaded() {
    if (!loaded) {
        user_count = user_load_all(users, MAX_USERS);
        loaded = 1;  // 设置已加载标志，后续调用直接使用内存数据
    }
}

/**
 * 调试函数：打印单个用户的详细信息到控制台
 * @param u 要打印的用户结构体指针
 */
void user_print(const User *u) {
    printf("ID:%d, Name:%s, Phone:%s, Balance:%.2f, Role:%s\n",
           u->id, u->name, u->phone, u->balance,
           u->role == ROLE_MANAGER ? "Manager" : "Customer");
}

/**
 * 根据用户ID查找用户
 * @param id 要查找的用户ID
 * @return 找到返回用户指针，未找到返回NULL
 */
User *user_find_by_id(int id) {
    ensure_loaded();  // 确保数据已加载
    // 遍历用户数组进行线性查找
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == id) return &users[i];
    }
    return NULL;
}

/**
 * 保存用户数据：更新现有用户或添加新用户
 * @param u 要保存的用户数据指针
 * @return 成功返回1，失败返回0
 * 
 * 工作流程：
 * 1. 如果是已存在用户（ID匹配），更新该用户信息
 * 2. 如果是新用户，添加到数组末尾
 * 3. 将整个数组写入文件持久化存储
 */
int user_save(const User *u) {
    ensure_loaded();
    // 查找是否已存在相同ID的用户（更新操作）
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == u->id) {
            users[i] = *u;  // 更新用户信息
            return user_write_all(users, user_count);  // 写入文件
        }
    }
    // 新用户添加操作
    if (user_count < MAX_USERS) {
        users[user_count++] = *u;  // 添加到数组并增加计数
        return user_write_all(users, user_count);  // 写入文件
    }
    printf("User count exceeds maximum limit!\n");
    return 0;
}

/**
 * 用户注册功能
 * @param role 用户角色（管理员或客户）
 * @return 成功返回新用户ID，失败返回-1
 * 
 * 注册流程：
 * 1. 自动生成用户ID（最后一个用户ID+1）
 * 2. 收集用户输入信息（姓名、手机号、密码）
 * 3. 初始余额设为0
 * 4. 保存到文件系统
 */
int user_register(UserRole role) {
    ensure_loaded();
    User newUser;
    // 生成新用户ID：如果没有任何用户从1开始，否则使用最后一个用户的ID+1
    newUser.id = (user_count == 0) ? 1 : users[user_count - 1].id + 1;
    newUser.balance = 0.0;    // 新用户初始余额为0
    newUser.role = role;      // 设置用户角色

    printf("\n=== User Registraction ===\n");
    printf("please input your name: ");
    scanf("%49s", newUser.name);      // 限制输入长度防止缓冲区溢出
    printf("please input your phone number: ");
    scanf("%14s", newUser.phone);     // 手机号长度限制
    printf("please set you password: ");
    scanf("%19s", newUser.password);  // 密码长度限制

    // 保存用户数据，如果失败返回错误
    if (!user_save(&newUser)) {
        printf("Registration failed, please check file path.\n");
        return -1;
    }
    printf("Registration successful! Your user ID is %d\n", newUser.id);
    return newUser.id;
}

/**
 * 用户登录功能
 * @param role 期望的用户角色，用于权限验证
 * @return 成功返回用户ID，失败返回-1
 * 
 * 登录验证流程：
 * 1. 验证用户ID是否存在
 * 2. 验证密码是否正确
 * 3. 验证用户角色是否匹配
 * 4. 全部验证通过返回用户ID
 */
int user_login(UserRole role) {
    ensure_loaded();
    int id;
    char pass[MAX_PASS_LEN];
    printf("\n=== User Login ===\n");
    printf("please input your ID: ");
    if (scanf("%d", &id) != 1) return -1;  // 读取ID，检查输入有效性
    printf("please input your password: ");
    scanf("%19s", pass);  // 读取密码

    // 根据ID查找用户
    User *u = user_find_by_id(id);
    if (!u) {
        printf("User does not exist!\n");
        return -1;
    }
    // 验证密码
    if (strcmp(u->password, pass) != 0) {
        printf("Wrong password!\n");
        return -1;
    }
    // 验证角色权限
    if (u->role != role) {
        printf("Permission error: Account role does not match.\n");
        return -1;
    }
    // 登录成功，显示欢迎信息
    printf("Welcome back,%s Current balance: %.2f\n", u->name, u->balance);
    return u->id;
}