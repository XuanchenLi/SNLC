
#include "mid.h"

//#include <iomanip>

using namespace std;

vector<Argnode> midtable;//四元式存储结构
vector<midprintnode> MidPrintNodeTable;//四元式输出表，存了4个String类型的变量

int lable = 0;//表示临时变量编号
int level = 0;//表示嵌套层级

ArgRecord::ArgRecord()
{

}

ArgRecord::ArgRecord(string _name) {
    this->name = _name;
}

ArgRecord* NewTemp(AccessKind access)//1.1 新建一个临时变量 
{
    auto t = new ArgRecord();
    t->form = "AddrForm";
    t->dataLevel = -1;
    t->dataOff = lable;
    t->access = access;
    string s = "temp";
    t->name += s;
    t->name += to_string(lable);

    lable += 1;
    return t;
}

ArgRecord* ARGValue(int value) {//1.2 新建数值类ARG结构 //pdf描述存疑
    auto t = new ArgRecord();
    t->form = "ValueForm";
    //t->form = "AddrForm";
    t->value = value;
    return t;
}

ArgRecord* ARGLabel(int label)//1.4 新建标号类ARG结构
{
    auto t = new ArgRecord();
    t->form = "LabelForm";
    t->label = label;
    return t;
}

ArgRecord* ARGAddr(const char* id, int level, int off, AccessKind access)//1.5 创建地址类ARG结构
{
    auto t = new ArgRecord();
    //t->form = "AddrForm";
    t->form = "ValueForm";
    // 变量名信息没写(现在写了)
    t->name = *id;
    t->dataLevel = level;
    t->dataOff = off;
    t->access = access;
    return t;
}

void PrintMidCode(vector<Argnode>& vec) {//1,6 输出中间代码
    cout << endl << "************************************** mid code **************************************" << endl;

    for (auto& s : MidPrintNodeTable) {
        cout << "( "
            << "\t" << std::left << setw(8)
            << s.op << " , "
            << "\t" << std::left << setw(8)
            << s.m1 << " , "
            << "\t" << std::left << setw(8)
            << s.m2 << " , "
            << "\t" << std::left << setw(8)
            << s.m3 << " ) " << endl;
    }
}

void GenCode(string codekind, ArgRecord* arg1, ArgRecord* arg2, ArgRecord* arg3)//1.7 生成中间代码
{
    string op = codekind;
    string n1 = arg1 ? arg1->name : "-";
    string n2 = arg2 ? arg2->name : "-";
    string n3 = arg3 ? arg3->name : "-";

    Argnode tmp;
    tmp.codekind = codekind;
    tmp.Arg1 = arg1;
    tmp.Arg2 = arg2;
    tmp.res = arg3;
    midtable.push_back(tmp);

    MidPrintNodeTable.push_back({ op, n1, n2, n3 });
}

string joint = "";

unordered_map<int, vector<pair<int, ASTNodeBase*>>> mapArray;//记录array的嵌套层级
unordered_map<int, vector<pair<int, ASTNodeBase*>>> mapRecord;//记录record的嵌套层级

void ProcessMid(ASTNodeBase* t)//中间代码生成的入口函数
{
    if (!t)
        return;
    auto t1 = t->child[0];// 程序头PheadK
    auto t2 = t->child[1];// 声明（TypeK、VarK、ProcK）
    auto t3 = t->child[2];// 程序体StmLK

    joint = GetBehind(t1);

    GenDeclare(t2);//调用GenDeclare()函数生成声明部分的中间代码
    if (t3) {//如果存在程序体，则调用GenBody()函数生成程序体部分的中间代码。
        GenBody(t3, GetBehind(t1));
    }
}

string GetBehind(ASTNodeBase* t)//用于获取给定AST节点后面的内容 
{
    string strs = "";

    ASTNodeKind str = t->nodeKind;
    if (str == ASTNodeKind::EXP_K)//检查节点类型，如果是表达式节点（ExpK），则提取其中的常量值
    {
        ASTExpNode* tmp = (ASTExpNode*)t;
        if (tmp->expKind == ASTEXPKind::CONST_K) {
            strs += to_string(tmp->expAttr.val);
        }
    }

    for (int i = 0; i < t->names.size(); i++)//遍历节点的名称数组，将名称拼接为字符串并返回
    {
        if (t->names[i] != "")
        {
            if (i != 0)
                strs += "";
            else
                strs += "";
            strs += t->names[i];
        }
    }
    return strs;
}

void GenDeclare(ASTNodeBase* t)//用于生成声明部分的中间代码
{
    ++level; //递增level变量，表示当前声明的嵌套层级
    if (!t)
        return;

    auto tmp = t;
    int flg = 0;
    while (tmp)//遍历声明节点，处理变量声明和函数声明
    {
        if (tmp->nodeKind == ASTNodeKind::PROC_K) {
            flg = 1;
            break;
        }
        if (tmp->nodeKind == ASTNodeKind::VAR_K)
        {
            auto tmp_child = tmp->child[0];
            while (tmp_child)
            {
                ASTDecNode* ntmp_child = (ASTDecNode*)tmp_child;
                ASTDecKind type = ntmp_child->decKind;
                //对于变量声明节点，根据不同的类型（ArrayK、RecordK）记录相关信息，如数组和记录的嵌套层级
                if (type == ASTDecKind::ARRAY_K) {
                    mapArray[level].push_back({ level, tmp_child });
                }
                if (type == ASTDecKind::RECORD_K) {
                    mapRecord[level].push_back({ level, tmp_child });
                }
                tmp_child = tmp_child->sibling;
            }

        }
        tmp = tmp->sibling;
    }

    if (flg) //如果存在函数声明节点（ProcK），调用GenProcDec()函数生成函数声明部分的中间代码
    {
        GenProcDec(tmp);
    }

    tmp = t;
    while (tmp)//继续遍历查找程序体节点（StmLK），并调用GenBody()函数生成程序体部分的中间代码
    {
        if (tmp->nodeKind == ASTNodeKind::STM_L_K) {
            GenBody(tmp, joint);
            break;
        }
        tmp = tmp->sibling;
    }
    return;
}

void GenProcDec(ASTNodeBase* ttt)//2.2 过程声明的中间代码生成
{
    ASTNodeBase* t = ttt->child[0];//注意此处语法树结构有所不同

    while (t)
    {
        if (!t)
            return;
        string f_name = GetBehind(t); // 得到函数的名字

        int p_flg = 0;

        // 先处理声明，这边递归处理声明
        if (t->nodeKind == ASTNodeKind::PROC_K) { // 声明中嵌套了函数声明
            p_flg = 1;
        }
        auto t1 = t->child[0];// 形参
        auto t2 = t->child[1];// 所有声明
        auto t3 = t->child[2];// 过程体
        // 正常情况是上面右边注释的那样，但实际上不是所有的函数都会有这三个child

        // 一共四种情况
        // 无参数 无声明
        if (!t1 && t2 && t2->nodeKind == ASTNodeKind::STM_L_K) {
            // 直接处理body
            GenBody(t2, f_name);
        }
        // 有参数 无声明
        else if (t1 && t1->nodeKind == ASTNodeKind::DEC_K && !t2 && t3 && t3->nodeKind == ASTNodeKind::STM_L_K) {
            // t1是参数部分 t3是body部分
            // 直接处理body
            GenBody(t3, f_name);
        }
        // 无参数 有声明
        else if (t1 && t2 && t2->nodeKind == ASTNodeKind::STM_L_K && !t3) {
            // t1是声明
            GenDeclare(t1->child[0]);
            // t2是body
            GenBody(t2, f_name); // 处理body
        }
        // 有参数 有声明
        else if (t1 && t1->nodeKind == ASTNodeKind::DEC_K && t2 && t3 && t3->nodeKind == ASTNodeKind::STM_L_K) {
            GenDeclare(t2->child[0]);
            GenDeclare(t2); // 加这个是因为如果在"有参数有声明"的情况下, t2的sibling有可能是会有另外的函数申明
            GenBody(t3, f_name);
        }

        if (p_flg) {
            // 之前创建过函数的入口
            GenCode("ENDPROC", new ArgRecord(f_name), nullptr, nullptr);//此处本注释掉
        }
        t = t->sibling; // 遍历剩下的函数声明
    }

}

// 处理函数body部分
void GenBody(ASTNodeBase* t)//2.3 语句体中间代码生成函数
{
    if (!t) {
        return;
    }

    // 指向第一个语句
    ASTNodeBase* t_sib = t->child[0];
    // 循环处理每一个body里的语句
    while (t_sib)
    {
        GenStatement(t_sib);
        t_sib = t_sib->sibling;
    }

    --level;
}

void GenBody(ASTNodeBase* t, string f_name)//重载2.3 用于生成程序体部分的中间代码
{
    if (!t) {
        return;
    }

    // 指向第一个语句
    ASTNodeBase* t_sib = t->child[0];
    // 循环处理每一个body里的语句
    GenCode("ENTRY", new ArgRecord(f_name), nullptr, nullptr);
    while (t_sib)
    {
        GenStatement(t_sib);
        t_sib = t_sib->sibling;
    }
    GenCode("ENDPROC", new ArgRecord(f_name), nullptr, nullptr);
    --level;
}

void GenStatement(ASTNodeBase* t)//2.4 语句的中间代码生成函数
{
    if (!t)
        return;
    // 一个语句
    if (t->nodeKind == ASTNodeKind::STMT_K)
    {
        ASTStmtNode* tmp = (ASTStmtNode*)t;
        ASTStmtKind operation = tmp->stmtKind;
        if (operation == ASTStmtKind::IF_K)
        {
            GenIfS(t);
        }
        else if (operation == ASTStmtKind::WHILE_K)
        {
            GenWhileS(t);
        }
        else if (operation == ASTStmtKind::ASSIGN_K)
        {
            GenAssignS(t);
        }
        else if (operation == ASTStmtKind::READ_K)
        {
            GenReadS(t);
        }
        else if (operation == ASTStmtKind::WRITE_K)
        {
            GenWriteS(t);
        }
        else if (operation == ASTStmtKind::CALL_K)
        {
            GenCallS(t);
        }
        else if (operation == ASTStmtKind::RETURN_K)
        {
            GenCode("RETURN", nullptr, nullptr, nullptr);
        }
        else
        {
            cout << "Error Match\n" << endl;
            system("pause");
        }
    }
}

void GenAssignS(ASTNodeBase* t)//2.5 赋值语句中间代码生成函数
{
    if (!t)
        return;

    auto first = t->child[0];
    auto second = t->child[1];

    auto Larg = ProcessQianTao(first);
    auto Rarg = ProcessQianTao(second);

    GenCode("ASSIGN", Rarg, nullptr, Larg);
}

ArgRecord* GenArray(ASTNodeBase* t) {//2.7 数组变量的中间代码生成函数

    ArgRecord* val = ProcessQianTao(t->child[0]); // 得到数组的val

    string arr_name = GetBehind(t);
    ASTNodeBase* tt = nullptr;

    for (int lev = level; lev >= 0; --lev)
    {
        for (int i = mapArray[lev].size() - 1; i >= 0; --i)
        {
            auto node = mapArray[lev][i].second;
            string node_name = GetBehind(node);
            if (node_name == arr_name) {
                tt = node;
                break;
            }
        }
    }

    if (!tt) {
        cout << "error" << endl;
    }

    ASTDecNode* ntmp = (ASTDecNode*)tt;
    int n_low = ntmp->decAttr.arrayAttr.low; // 得到下标的最小数值
    int n_high = ntmp->decAttr.arrayAttr.up;

    string s_low = to_string(n_low);
    ArgRecord* t1 = NewTemp(AccessKind::didr);
    GenCode("-", val, new ArgRecord(s_low), t1);
    ASTDecKind type = ntmp->decAttr.arrayAttr.itemType;
    string stype = "";//把ASTDecKind类型的type转为String
    if (type == ASTDecKind::INTEGER_K)
        //stype = "INTEGER_KSize";
        stype = "Size";
    else if (type == ASTDecKind::CHAR_K)
        //stype = "CHAR_KSize";
        stype = "Size";
    ArgRecord* t2 = NewTemp(AccessKind::didr);
    ArgRecord* t3 = NewTemp(AccessKind::didr);
    GenCode("*", t1, new ArgRecord(stype), t2);
    GenCode("[+]", new ArgRecord(arr_name), t2, t3);

    return t3;
}

ArgRecord* GenField(ASTNodeBase* t)//2.8 域成员变量的中间代码生成函数
{
    if (!t)
        return nullptr;

    auto t_1 = t->child[0];
    auto r = NewTemp(AccessKind::didr);
    string bianlaing = "Off" + GetBehind(t_1);
    GenCode("[+]", new ArgRecord(GetBehind(t)), new ArgRecord(bianlaing), r);
    return r;

}

ArgRecord* ProcessQianTao(ASTNodeBase* t)//集成了2.6 2.9
{
    if (!t)
        return nullptr;

    ASTExpNode* ntmp = (ASTExpNode*)t;
    ASTEXPKind exp = ntmp->expKind;

    // 暂时只考虑这两个情况,标识符或者常数时
    if (exp == ASTEXPKind::ID_K || exp == ASTEXPKind::CONST_K)
    {
        ASTVarType yifangwanyi = ntmp->expAttr.varType;
        if (exp == ASTEXPKind::ID_K && yifangwanyi == ASTVarType::ARRAY_MEMB_V) {
            // 如果这个条件满足，说明是数组的开始，特殊处理
            auto m = GenArray(t);
            return m;
        }
        else if (exp == ASTEXPKind::ID_K && yifangwanyi == ASTVarType::FIELD_MEMB_V) { // field
            return GenField(t);
        }
        else
        {
            // 说明到终点了，不用递归了
            return new ArgRecord(GetBehind(t));
        }
    }
    else
    {
        // 如这里是OpK的话 先去处理两个操作分量
        auto t1 = ProcessQianTao(t->child[0]);
        auto t2 = ProcessQianTao(t->child[1]);
        // 得到具体的操作是什么
        ASTExpNode* ntmp = (ASTExpNode*)t;
        ASTOpType op = ntmp->expAttr.op;
        string str = "";
        if (op == ASTOpType::LT)
            str += "< ";
        else if (op == ASTOpType::EQ)
            str += "= ";
        else if (op == ASTOpType::PLUS)
            str += "+ ";
        else if (op == ASTOpType::MINUS)
            str += "- ";
        else if (op == ASTOpType::TIMES)
            str += "* ";
        else if (op == ASTOpType::OVER)
            str += "/ ";

        auto r = NewTemp(AccessKind::didr); // 创建一个temp节点
        GenCode(str, t1, t2, r);
        return r; // 把创建的节点返回
    }
}

// 只处理了值参的情况
void GenCallS(ASTNodeBase* t)//2.10 过程调用语句的中间代码生成函数
{
    auto t_1 = t->child[0]; // 这里应该是有过程的名字
    string t_1_name = GetBehind(t_1);

    auto tmp = t->child[1]; // 确定了是child[1]，就是说函数名字在child[0]里，但是传入的参数在child[1]中

    int offset = 0;
    while (tmp)
    {
        string na = GetBehind(tmp);

        // 暂时先当数 参处理
        GenCode("ValACT", new ArgRecord(na), new ArgRecord(to_string(offset)), new ArgRecord(to_string(1)));
        offset += 1; // 偏移暂时先当1处理
        tmp = tmp->sibling;
    }
    GenCode("CALL", new ArgRecord(t_1_name), new ArgRecord("true"), new ArgRecord("NULL"));
}

void GenReadS(ASTNodeBase* t)//2.11 读语句中间代码生成函数
{
    auto tmp = ARGAddr((GetBehind(t)).data(), 0, 0, AccessKind::didr);
    GenCode("READC", new ArgRecord(GetBehind(t)), nullptr, nullptr);
}

void GenIfS(ASTNodeBase* t)//2.12 条件语句中间代码生成函数
{
    if (!t)
        return;
    auto t_1 = t->child[0]; // if(A)的A的部分
    auto res = ProcessQianTao(t_1);
    GenCode("THEN", res, nullptr, nullptr);

    // if后else前的部分
    auto t_2 = t->child[1];
    auto tmp = t_2;
    while (tmp) {
        GenStatement(tmp);
        tmp = tmp->sibling;
    }
    // else后，这部分可能没有

    auto t_3 = t->child[2];

    if (t_3)
    {
        GenCode("ELSE", nullptr, nullptr, nullptr);
        tmp = t_3;
        while (tmp) {
            GenStatement(tmp);
            tmp = tmp->sibling;
        }
    }
    GenCode("ENDIF", nullptr, nullptr, nullptr);
}


void GenWriteS(ASTNodeBase* t)//2.13 写语句中间代码生成函数
{
    // auto tmp = GenExpr(t->child[0]);
    auto tmp = ProcessQianTao(t->child[0]);
    GenCode("WRITEC", tmp, nullptr, nullptr);
}

// 同if
void GenWhileS(ASTNodeBase* t)//2.14 循环语句中间代码生成函数
{
    GenCode("WHILE", nullptr, nullptr, nullptr);
    auto t_1 = t->child[0];
    auto res = ProcessQianTao(t_1);
    GenCode("DO", res, nullptr, nullptr);

    // while() {A} A的部分
    auto t_2 = t->child[1];
    auto tmp = t_2;
    while (tmp)
    {
        GenStatement(tmp);
        tmp = tmp->sibling;
    }
    GenCode("ENDWHILE", nullptr, nullptr, nullptr);
}
