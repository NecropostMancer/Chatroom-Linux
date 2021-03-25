/****************************************

权限，未实现

****************************************/

#ifndef PERMISSION_H
#define PERMISSION_H

#include "User.h"

#define CAN_KICK 1 << 0;
#define CAN_LOCK 1 << 1;
#define CAN_SAY 1 << 2;

class Permission
{
    public:
        Permission();
        virtual ~Permission();
        bool HasPermission(User user,Room room,int permission);
        bool AddPermission(User user,Room room,int permission);
    protected:

    private:
};

#endif // PERMISSION_H
