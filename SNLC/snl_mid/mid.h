#pragma once
#include<iomanip>
#include <unordered_map>
#include "../snl_semantic/symTable.h"
#include "../snl_semantic/table.h"
#include "../snl_common/ast.h"


using namespace std;

struct ArgRecord//ArgRecord�ṹ
{
    ArgRecord();
    ArgRecord(string _name);
    string form; // ȡֵ��ValueForm, LabelForm, AddrForm
    int value; // formȡֵValueFormʱ��Ч ��¼ ���� ����ֵ
    int label; // formȡֵLabelFormʱ��Ч ��¼ ��� ����ֵ
    //string Addr; // ������ArgRecord�ṹ������
    string name; // ��¼���������� ��Ϊ��ʱ��������Ϊ��
    int dataLevel; // ��¼������ʶ�����ڵĲ��� ��Ϊ��ʱ������ȡ-1
    int dataOff;// ��¼�����ı�ʶ����ƫ���� ������ʱ���� ȡֵ��ʱ�����ı���
    AccessKind access; // ��¼�����ķ��ʷ�ʽ��������ֱ�ӻ��Ƿ�ֱ�� enum AccessKind { didr,indir };
};

struct Argnode//�м����ṹ
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

extern int lable;//��ʾ��ʱ�������
extern int level;//��ʾǶ�ײ㼶

extern ArgRecord* NewTemp(AccessKind access);//1.1 �½�һ����ʱ����
extern ArgRecord* ARGValue(int value);//1.2 �½���ֵ��ARG�ṹ
extern int NewLabel();//1.3 ����һ���µı��
extern ArgRecord* ARGLabel(int lable);//1.4 �½������ARG�ṹ
extern ArgRecord* ARGAddr(char* id, int level, int Off, AccessKind access);//1.5 ������ַ��ARG�ṹ
extern void PrintMidCode(vector<Argnode>& vec);//1,6 ����м����
extern void GenCode(vector<Argnode>& vec, string codekind, ArgRecord* arg1, ArgRecord* arg2, ArgRecord* arg3);//1.7 �����м���� 

extern void GenProcDec(ASTNodeBase*);//2.2 �����������м��������
extern void GenBody(ASTNodeBase* t);//2.3 ������м�������ɺ���
extern void GenStatement(ASTNodeBase* t);//2.4 �����м�������ɺ���
extern void GenAssignS(ASTNodeBase* t);//2.5 ��ֵ����м�������ɺ���
extern ArgRecord* GenArray(ASTNodeBase* t);//2.7 ����������м�������ɺ���
extern ArgRecord* GenField(ASTNodeBase* t);//2.8 ���Ա�������м�������ɺ���
extern void GenCallS(ASTNodeBase* t);//2.10 ���̵��������м�������ɺ���
extern void GenReadS(ASTNodeBase* t);//2.11 ������м�������ɺ���
extern void GenIfS(ASTNodeBase* t);//2.12 ��������м�������ɺ���
extern void GenWriteS(ASTNodeBase* t);//2.13 д����м�������ɺ���
extern void GenWhileS(ASTNodeBase* t);//2.14 ѭ������м�������ɺ���
//
//extern vector<Argnode>& vec GenMidCode(ASTNodeBase* t);//2.1 �м��������������
//extern ArgRecord* GenVar(ASTNodeBase* t);//2.6 �����м�������ɺ���
//extern ArgRecord* GenExpr(ASTNodeBase* t);//2.9 ���ʽ���м�������ɺ���


extern void ProcessMid(ASTNodeBase* t); //�м�������ɵ���ں���
extern string GetBehind(ASTNodeBase* t);//���ڻ�ȡ����AST�ڵ��������� 
extern void GenDeclare(ASTNodeBase* t);//���������������ֵ��м����
extern void GenBody(ASTNodeBase* t, string);//�������ɳ����岿�ֵ��м����
extern ArgRecord* ProcessQianTao(ASTNodeBase* t);

