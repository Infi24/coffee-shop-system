#include "user.h"
#include <assert.h>
#include <stdio.h>

// 测试辅助函数
void divider(const char *msg) {
    printf("\n-----------------------------\n%s\n-----------------------------\n", msg);
}

int main() {
    printf("=== Running user module tests ===\n");

    // 1️ 注册测试
    divider("TEST 1: Register new customer");
    int id = user_register(ROLE_CUSTOMER);
    assert(id > 0);

    // 2️ 登录成功测试
    divider("TEST 2: Login with correct password");
    int login_id = user_login(ROLE_CUSTOMER);
    assert(login_id == id);

    // 3️ 登录失败测试（错误密码）
    divider("TEST 3: Login with wrong password");
    int wrong_id = user_login(ROLE_CUSTOMER);
    assert(wrong_id == -1);

    // 4️ 店长注册+登录
    divider("TEST 4: Manager register and login");
    int mgr_id = user_register(ROLE_MANAGER);
    int mgr_login = user_login(ROLE_MANAGER);
    assert(mgr_login == mgr_id);

    // 5 查询与保存测试
    divider("TEST 5: Find and update user");
    User *u = user_find_by_id(id);
    assert(u != NULL);
    u->balance += 10.0;
    assert(user_save(u) == 1);

    printf("\nAll tests passed successfully!\n");
    return 0;
}
