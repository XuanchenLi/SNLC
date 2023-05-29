
#include "mid.h"

//#include <iomanip>

using namespace std;

vector<Argnode> midtable;//��Ԫʽ�洢�ṹ
vector<midprintnode> MidPrintNodeTable;//��Ԫʽ���������4��String���͵ı���

int lable = 0;//��ʾ��ʱ�������
int level = 0;//��ʾǶ�ײ㼶

ArgRecord::ArgRecord()
{

}

ArgRecord::ArgRecord(string _name) {
    this->name = _name;
}

ArgRecord* NewTemp(AccessKind access)//1.1 �½�һ����ʱ���� 
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

ArgRecord* ARGValue(int value) {//1.2 �½���ֵ��ARG�ṹ //pdf��������
    auto t = new ArgRecord();
    t->form = "ValueForm";
    //t->form = "AddrForm";
    t->value = value;
    return t;
}

ArgRecord* ARGLabel(int label)//1.4 �½������ARG�ṹ
{
    auto t = new ArgRecord();
    t->form = "LabelForm";
    t->label = label;
    return t;
}

ArgRecord* ARGAddr(const char* id, int level, int off, AccessKind access)//1.5 ������ַ��ARG�ṹ
{
    auto t = new ArgRecord();
    //t->form = "AddrForm";
    t->form = "ValueForm";
    // ��������Ϣûд(����д��)
    t->name = *id;
    t->dataLevel = level;
    t->dataOff = off;
    t->access = access;
    return t;
}

void PrintMidCode(vector<Argnode>& vec) {//1,6 ����м����
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

void GenCode(string codekind, ArgRecord* arg1, ArgRecord* arg2, ArgRecord* arg3)//1.7 �����м����
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

unordered_map<int, vector<pair<int, ASTNodeBase*>>> mapArray;//��¼array��Ƕ�ײ㼶
unordered_map<int, vector<pair<int, ASTNodeBase*>>> mapRecord;//��¼record��Ƕ�ײ㼶

void ProcessMid(ASTNodeBase* t)//�м�������ɵ���ں���
{
    if (!t)
        return;
    auto t1 = t->child[0];// ����ͷPheadK
    auto t2 = t->child[1];// ������TypeK��VarK��ProcK��
    auto t3 = t->child[2];// ������StmLK

    joint = GetBehind(t1);

    GenDeclare(t2);//����GenDeclare()���������������ֵ��м����
    if (t3) {//������ڳ����壬�����GenBody()�������ɳ����岿�ֵ��м���롣
        GenBody(t3, GetBehind(t1));
    }
}

string GetBehind(ASTNodeBase* t)//���ڻ�ȡ����AST�ڵ��������� 
{
    string strs = "";

    ASTNodeKind str = t->nodeKind;
    if (str == ASTNodeKind::EXP_K)//���ڵ����ͣ�����Ǳ��ʽ�ڵ㣨ExpK��������ȡ���еĳ���ֵ
    {
        ASTExpNode* tmp = (ASTExpNode*)t;
        if (tmp->expKind == ASTEXPKind::CONST_K) {
            strs += to_string(tmp->expAttr.val);
        }
    }

    for (int i = 0; i < t->names.size(); i++)//�����ڵ���������飬������ƴ��Ϊ�ַ���������
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

void GenDeclare(ASTNodeBase* t)//���������������ֵ��м����
{
    ++level; //����level��������ʾ��ǰ������Ƕ�ײ㼶
    if (!t)
        return;

    auto tmp = t;
    int flg = 0;
    while (tmp)//���������ڵ㣬������������ͺ�������
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
                //���ڱ��������ڵ㣬���ݲ�ͬ�����ͣ�ArrayK��RecordK����¼�����Ϣ��������ͼ�¼��Ƕ�ײ㼶
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

    if (flg) //������ں��������ڵ㣨ProcK��������GenProcDec()�������ɺ����������ֵ��м����
    {
        GenProcDec(tmp);
    }

    tmp = t;
    while (tmp)//�����������ҳ�����ڵ㣨StmLK����������GenBody()�������ɳ����岿�ֵ��м����
    {
        if (tmp->nodeKind == ASTNodeKind::STM_L_K) {
            GenBody(tmp, joint);
            break;
        }
        tmp = tmp->sibling;
    }
    return;
}

void GenProcDec(ASTNodeBase* ttt)//2.2 �����������м��������
{
    ASTNodeBase* t = ttt->child[0];//ע��˴��﷨���ṹ������ͬ

    while (t)
    {
        if (!t)
            return;
        string f_name = GetBehind(t); // �õ�����������

        int p_flg = 0;

        // �ȴ�����������ߵݹ鴦������
        if (t->nodeKind == ASTNodeKind::PROC_K) { // ������Ƕ���˺�������
            p_flg = 1;
        }
        auto t1 = t->child[0];// �β�
        auto t2 = t->child[1];// ��������
        auto t3 = t->child[2];// ������
        // ��������������ұ�ע�͵���������ʵ���ϲ������еĺ���������������child

        // һ���������
        // �޲��� ������
        if (!t1 && t2 && t2->nodeKind == ASTNodeKind::STM_L_K) {
            // ֱ�Ӵ���body
            GenBody(t2, f_name);
        }
        // �в��� ������
        else if (t1 && t1->nodeKind == ASTNodeKind::DEC_K && !t2 && t3 && t3->nodeKind == ASTNodeKind::STM_L_K) {
            // t1�ǲ������� t3��body����
            // ֱ�Ӵ���body
            GenBody(t3, f_name);
        }
        // �޲��� ������
        else if (t1 && t2 && t2->nodeKind == ASTNodeKind::STM_L_K && !t3) {
            // t1������
            GenDeclare(t1->child[0]);
            // t2��body
            GenBody(t2, f_name); // ����body
        }
        // �в��� ������
        else if (t1 && t1->nodeKind == ASTNodeKind::DEC_K && t2 && t3 && t3->nodeKind == ASTNodeKind::STM_L_K) {
            GenDeclare(t2->child[0]);
            GenDeclare(t2); // ���������Ϊ�����"�в���������"�������, t2��sibling�п����ǻ�������ĺ�������
            GenBody(t3, f_name);
        }

        if (p_flg) {
            // ֮ǰ���������������
            GenCode("ENDPROC", new ArgRecord(f_name), nullptr, nullptr);//�˴���ע�͵�
        }
        t = t->sibling; // ����ʣ�µĺ�������
    }

}

// ������body����
void GenBody(ASTNodeBase* t)//2.3 ������м�������ɺ���
{
    if (!t) {
        return;
    }

    // ָ���һ�����
    ASTNodeBase* t_sib = t->child[0];
    // ѭ������ÿһ��body������
    while (t_sib)
    {
        GenStatement(t_sib);
        t_sib = t_sib->sibling;
    }

    --level;
}

void GenBody(ASTNodeBase* t, string f_name)//����2.3 �������ɳ����岿�ֵ��м����
{
    if (!t) {
        return;
    }

    // ָ���һ�����
    ASTNodeBase* t_sib = t->child[0];
    // ѭ������ÿһ��body������
    GenCode("ENTRY", new ArgRecord(f_name), nullptr, nullptr);
    while (t_sib)
    {
        GenStatement(t_sib);
        t_sib = t_sib->sibling;
    }
    GenCode("ENDPROC", new ArgRecord(f_name), nullptr, nullptr);
    --level;
}

void GenStatement(ASTNodeBase* t)//2.4 �����м�������ɺ���
{
    if (!t)
        return;
    // һ�����
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

void GenAssignS(ASTNodeBase* t)//2.5 ��ֵ����м�������ɺ���
{
    if (!t)
        return;

    auto first = t->child[0];
    auto second = t->child[1];

    auto Larg = ProcessQianTao(first);
    auto Rarg = ProcessQianTao(second);

    GenCode("ASSIGN", Rarg, nullptr, Larg);
}

ArgRecord* GenArray(ASTNodeBase* t) {//2.7 ����������м�������ɺ���

    ArgRecord* val = ProcessQianTao(t->child[0]); // �õ������val

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
    int n_low = ntmp->decAttr.arrayAttr.low; // �õ��±����С��ֵ
    int n_high = ntmp->decAttr.arrayAttr.up;

    string s_low = to_string(n_low);
    ArgRecord* t1 = NewTemp(AccessKind::didr);
    GenCode("-", val, new ArgRecord(s_low), t1);
    ASTDecKind type = ntmp->decAttr.arrayAttr.itemType;
    string stype = "";//��ASTDecKind���͵�typeתΪString
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

ArgRecord* GenField(ASTNodeBase* t)//2.8 ���Ա�������м�������ɺ���
{
    if (!t)
        return nullptr;

    auto t_1 = t->child[0];
    auto r = NewTemp(AccessKind::didr);
    string bianlaing = "Off" + GetBehind(t_1);
    GenCode("[+]", new ArgRecord(GetBehind(t)), new ArgRecord(bianlaing), r);
    return r;

}

ArgRecord* ProcessQianTao(ASTNodeBase* t)//������2.6 2.9
{
    if (!t)
        return nullptr;

    ASTExpNode* ntmp = (ASTExpNode*)t;
    ASTEXPKind exp = ntmp->expKind;

    // ��ʱֻ�������������,��ʶ�����߳���ʱ
    if (exp == ASTEXPKind::ID_K || exp == ASTEXPKind::CONST_K)
    {
        ASTVarType yifangwanyi = ntmp->expAttr.varType;
        if (exp == ASTEXPKind::ID_K && yifangwanyi == ASTVarType::ARRAY_MEMB_V) {
            // �������������㣬˵��������Ŀ�ʼ�����⴦��
            auto m = GenArray(t);
            return m;
        }
        else if (exp == ASTEXPKind::ID_K && yifangwanyi == ASTVarType::FIELD_MEMB_V) { // field
            return GenField(t);
        }
        else
        {
            // ˵�����յ��ˣ����õݹ���
            return new ArgRecord(GetBehind(t));
        }
    }
    else
    {
        // ��������OpK�Ļ� ��ȥ����������������
        auto t1 = ProcessQianTao(t->child[0]);
        auto t2 = ProcessQianTao(t->child[1]);
        // �õ�����Ĳ�����ʲô
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

        auto r = NewTemp(AccessKind::didr); // ����һ��temp�ڵ�
        GenCode(str, t1, t2, r);
        return r; // �Ѵ����Ľڵ㷵��
    }
}

// ֻ������ֵ�ε����
void GenCallS(ASTNodeBase* t)//2.10 ���̵��������м�������ɺ���
{
    auto t_1 = t->child[0]; // ����Ӧ�����й��̵�����
    string t_1_name = GetBehind(t_1);

    auto tmp = t->child[1]; // ȷ������child[1]������˵����������child[0]����Ǵ���Ĳ�����child[1]��

    int offset = 0;
    while (tmp)
    {
        string na = GetBehind(tmp);

        // ��ʱ�ȵ��� �δ���
        GenCode("ValACT", new ArgRecord(na), new ArgRecord(to_string(offset)), new ArgRecord(to_string(1)));
        offset += 1; // ƫ����ʱ�ȵ�1����
        tmp = tmp->sibling;
    }
    GenCode("CALL", new ArgRecord(t_1_name), new ArgRecord("true"), new ArgRecord("NULL"));
}

void GenReadS(ASTNodeBase* t)//2.11 ������м�������ɺ���
{
    auto tmp = ARGAddr((GetBehind(t)).data(), 0, 0, AccessKind::didr);
    GenCode("READC", new ArgRecord(GetBehind(t)), nullptr, nullptr);
}

void GenIfS(ASTNodeBase* t)//2.12 ��������м�������ɺ���
{
    if (!t)
        return;
    auto t_1 = t->child[0]; // if(A)��A�Ĳ���
    auto res = ProcessQianTao(t_1);
    GenCode("THEN", res, nullptr, nullptr);

    // if��elseǰ�Ĳ���
    auto t_2 = t->child[1];
    auto tmp = t_2;
    while (tmp) {
        GenStatement(tmp);
        tmp = tmp->sibling;
    }
    // else���ⲿ�ֿ���û��

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


void GenWriteS(ASTNodeBase* t)//2.13 д����м�������ɺ���
{
    // auto tmp = GenExpr(t->child[0]);
    auto tmp = ProcessQianTao(t->child[0]);
    GenCode("WRITEC", tmp, nullptr, nullptr);
}

// ͬif
void GenWhileS(ASTNodeBase* t)//2.14 ѭ������м�������ɺ���
{
    GenCode("WHILE", nullptr, nullptr, nullptr);
    auto t_1 = t->child[0];
    auto res = ProcessQianTao(t_1);
    GenCode("DO", res, nullptr, nullptr);

    // while() {A} A�Ĳ���
    auto t_2 = t->child[1];
    auto tmp = t_2;
    while (tmp)
    {
        GenStatement(tmp);
        tmp = tmp->sibling;
    }
    GenCode("ENDWHILE", nullptr, nullptr, nullptr);
}
