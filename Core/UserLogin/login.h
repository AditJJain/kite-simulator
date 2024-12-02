#ifndef LOGIN_H
#define LOGIN_H

#include <string>

class login {
public:
    void Login();
    void Registration();
    void ForgotPassword();
    void DeleteLine(std::string userDelete);
};

void runUserLogin();

#endif // LOGIN_H