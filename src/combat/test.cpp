
#include "../../include/combat.h"
#include <windows.h>
#include "../../include/equip.h"
#include "../../lib/nterm.h"

#include <iostream>
int main()
{
    ntermInit();
    /*
    #ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    #endif*/
    srand((unsigned int)time(NULL));
    Skill s1("爪击", "大凶兔气势汹汹的一击，威力不可小觑。", 20, 0);
    Skill S[1] = { s1 };
    Skill s2("凌天一斩", "奋力向对方发动一次斩击。 ", 40, 20);
    Enemy r("大凶兔", "本来只爱吃胡萝卜的兔子不知为何变得凶巴巴的，小心！它会攻击你的！", 50, 50, 20, 20, 15, 5, 1, 5, 4);
    r.setSkill(S, 1);
    Hero h;
    h.setName();
    h.setSkill(s2);
    std::shared_ptr<RedMedicine> redMedicine = std::make_shared<RedMedicine>() ;
    h.getBag().get(redMedicine,10);
    Fight(&h);
    return 0;
}
