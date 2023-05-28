#pragma once
#include<iomanip>
#include <unordered_map>
#include "../snl_semantic/symTable.h"
#include "../snl_semantic/table.h"
#include "../snl_common/ast.h"


using namespace std;

struct ArgRecord//ArgRecord结构
{
    ArgRecord();
    ArgRecord(string _name);
    string form; // 取值是ValueForm, LabelForm, AddrForm
    int value; // form取值ValueForm时有效 记录 常数 的数值
    int label; // form取值LabelForm时有效 记录 标号 的数值
    //string Addr; // 变量的ArgRecord结构的内容
    string name; // 记录变量的名字 若为临时变量，则为空
    int dataLevel; // 记录变量标识符所在的层数 若为临时变量，取-1
    int dataOff;// 记录变量的标识符的偏移量 若是临时变量 取值临时变量的编码
    AccessKind access; // 记录变量的访问方式，根据是直接还是非直接 enum AccessKind { didr,indir };
};

struct Argnode//中间代码结构
{
    string codekind;
    ArgRecord* Arg1;
    ArgRecord* Arg2;
    ArgRecord* res;
};
extern vector<Argnode> midtable;

struct midprintnode
{
    string op;
    string m1;
    string m2;
    string m3;
};
extern vector<midprintnode> MidPrintNodeTable;

//extern vector<ASTNodeBase*> ArrayTable;
//extern vector<ASTNodeBase*> FieldTable;

extern unordered_map<int, vector<pair<int, ASTNodeBase*>>> mapArray;
extern unordered_map<int, vector<pair<int, ASTNodeBase*>>> mapRecord;

extern int lable;//表示临时变量编号
extern int level;//表示嵌套层级

extern ArgRecord* NewTemp(AccessKind access);//1.1 新建一个临时变量
extern ArgRecord* ARGValue(int value);//1.2 新建数值类ARG结构
extern int NewLabel();//1.3 产生一个新的标号
extern ArgRecord* ARGLabel(int lable);//1.4 新建标号类ARG结构
extern ArgRecord* ARGAddr(char* id, int level, int Off, AccessKind access);//1.5 创建地址类ARG结构
extern void PrintMidCode(vector<Argnode>& vec);//1,6 输出中间代码
extern void GenCode(vector<Argnode>& vec, string codekind, ArgRecord* arg1, ArgRecord* arg2, ArgRecord* arg3);//1.7 生成中间代码 

extern void GenProcDec(ASTNodeBase*);//2.2 过程声明的中间代码生成
extern void GenBody(ASTNodeBase* t);//2.3 语句体中间代码生成函数
extern void GenStatement(ASTNodeBase* t);//2.4 语句的中间代码生成函数
extern void GenAssignS(ASTNodeBase* t);//2.5 赋值语句中间代码生成函数
extern ArgRecord* GenArray(ASTNodeBase* t);//2.7 数组变量的中间代码生成函数
extern ArgRecord* GenField(ASTNodeBase* t);//2.8 域成员变量的中间代码生成函数
extern void GenCallS(ASTNodeBase* t);//2.10 过程调用语句的中间代码生成函数
extern void GenReadS(ASTNodeBase* t);//2.11 读语句中间代码生成函数
extern void GenIfS(ASTNodeBase* t);//2.12 条件语句中间代码生成函数
extern void GenWriteS(ASTNodeBase* t);//2.13 写语句中间代码生成函数
extern void GenWhileS(ASTNodeBase* t);//2.14 循环语句中间代码生成函数
//
//extern vector<Argnode>& vec GenMidCode(ASTNodeBase* t);//2.1 中间代码生成主函数
//extern ArgRecord* GenVar(ASTNodeBase* t);//2.6 变量中间代码生成函数
//extern ArgRecord* GenExpr(ASTNodeBase* t);//2.9 表达式的中间代码生成函数


extern void ProcessMid(ASTNodeBase* t); //中间代码生成的入口函数
extern string GetBehind(ASTNodeBase* t);//用于获取给定AST节点后面的内容 
extern void GenDeclare(ASTNodeBase* t);//用于生成声明部分的中间代码
extern void GenBody(ASTNodeBase* t, string);//用于生成程序体部分的中间代码
extern ArgRecord* ProcessQianTao(ASTNodeBase* t);

