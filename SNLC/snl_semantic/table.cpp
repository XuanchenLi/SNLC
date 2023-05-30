#include "table.h"


Table::~Table() {


}
void Table::CreatTable() {
	/*
		������һ���µľֲ�����λʱ�����ñ��ӳ��򡣹����ǽ���һ����
		���ű�table,������1,ƫ�Ƴ�ʼ��Ϊ0��
	*/
	this->Level++;
	std::vector<symTablePtr> newitem;
	this->scope.push_back(newitem);
}

void Table::DestroyTable() {
	/*
		����һ�����ű�
		�ӷֳ�����е�����������ű���
	*/
	this->Access++;
	this->tables.push_back(this->scope.back());

	this->Level--;
	this->scope.pop_back();

}

bool Table::Enter(char* Id, AttributelR* AttribP, symTablePtr &Entry) {
	/*
		����ʶ��������ʶ�����ԵǼ��ڷ��ű��У��Ǽǳɹ�������ֵΪ
		�棬Entryָ��Ǽǵı�ʶ���ڷ��ű��е�λ�ã��Ǽǲ��ɹ�������
		ֵΪ�٣�EntryֵΪ�ա�
	*/
	if (FindEntry(Id, false , Entry)) {
		//�����������
		throw std::runtime_error(
			strcat(Id, " declare twice")
		);
	}

	symTablePtr item = new symTable();
	strcpy(item->idname, Id);
	item->next = nullptr;
	item->attrIR = *AttribP; //ǳ����
	
	this->scope.at(this->Level - 1).push_back(item);

	Entry = item;
	return true;
}

bool Table::FindEntry(char* id, bool diraction, symTablePtr &Entry) {
	/*
		directionΪ������ײ����ң�Ϊ���ڱ����ڲ���
		����flag��ֵΪone����total,�����ڵ�ǰ���ű��в��ұ�ʶ����
		������scopeջ�е����з��ű��в��ұ�ʶ����
	*/
	if (diraction) {
		//Ϊ����ײ�����
		for (int level = this->Level-1; level >= 0; level--) {
			if (SearchoneTable(id, level, Entry)) return true;
		}
	}
	else {
		//Ϊ�ٱ����ڲ���
		if (SearchoneTable(id, this->Level - 1, Entry)) return true;
	}
	Entry = nullptr;
	return false;
}

bool Table::SearchoneTable(char* id, int currentLevel, symTablePtr &Entry) {
	/*
		�ӱ�ͷ��ʼ�����ν��ڵ��еı�ʶ�����ֺ�id�Ƚ��Ƿ���ͬ��ֱ
		���ҵ��˱�ʶ���򵽴��β�����ҵ���������ֵ��EntryΪ��ʶ��
		�ڷ��ű��е�λ�ã����򣬷���ֵΪ�١�
	*/
	std::vector<symTablePtr> &table = this->scope.at(currentLevel);

	for (int i = 0; i < table.size(); i++) {
		if (strcmp(table.at(i)->idname, id) == 0) {
			Entry = table.at(i);
			return true;
		}
	}
	Entry = nullptr;
	return false;
}

void Table::Analyze(ASTNodeBase* currentP) {
	this->analyze(currentP);
	if (this->Access) this->PrintSymbTabl();
}
void Table::initialize() {
	/*
		��ʼ���������ͣ��ַ����ͣ��������͵��ڲ���ʾ˵������������
		���;�Ϊ�������ͣ��ڲ���ʾ�̶���
	*/

	typeIR* item = new typeIR;
	item->kind = intTy;
	item->size = 1;
	this->typetabel["intTy"] = item;

	item = new typeIR;
	item->kind = charTy;
	item->size = 1;
	this->typetabel["charTy"] = item;

	item = new typeIR;
	item->kind = boolTy;
	item->size = 1;
	this->typetabel["boolTy"] = item;

}

TypeIR* Table::TypeProcess(ASTNodeBase* currentP) {
	/*
		���ͷ������������﷨���ĵ�ǰ�ڵ����ͣ����쵱ǰ���͵��ڲ�
		��ʾ���������ַ���ظ�Ptr�����ڲ���ʾ�ĵ�ַ��
	*/
	 ASTDecNode* tmp = (ASTDecNode*)currentP;
	switch (tmp->decKind)
	{
	case ASTDecKind::ID_K:
		return this->nameType(currentP);
		break;
	case ASTDecKind::RECORD_K:
		return this->recordType(currentP);
		break;
	case ASTDecKind::ARRAY_K:
		return this->arrayType(tmp);
		break;
	case ASTDecKind::INTEGER_K:
		return this->typetabel.find("intTy")->second; break;
	case ASTDecKind::CHAR_K:
		return this->typetabel.find("intTy")->second; break;
	default:
		break;
	}

}

TypeIR* Table::arrayType(ASTDecNode* currentP) {
	/*
		�����������͵��ڲ���ʾ����ʱ�����±��Ƿ�Խ��
	*/
	if (currentP->decAttr.arrayAttr.up < currentP->decAttr.arrayAttr.low) {
		std::cout << "over boundary" << std:: endl;
		return nullptr;
	}

	typeIR* item = new typeIR;
	item->kind = arrayTy;
	item->size = currentP->decAttr.arrayAttr.up - currentP->decAttr.arrayAttr.low + 1;
	if (currentP->decAttr.arrayAttr.itemType == ASTDecKind::INTEGER_K) item->More.ArrayAttr.elemTy = this->typetabel["intTy"];
	else item->More.ArrayAttr.elemTy = this->typetabel["charTy"];
	//�±�����Ĭ��Ϊ����
	item->More.ArrayAttr.indexTy= this->typetabel["intTy"];
	//�����ڲ����ͱ���
	this->typetabel.insert(std::pair<std::string, TypeIR*>(currentP->nodeBase.names.back(), item));
	return item;
}


void Table::VarDecList(ASTNodeBase* currentP) {
	/*
		������������ʶ��idʱ����id�Ǽǵ����ű��У�����ظ��Զ��壻
		��������ʱ���������ڲ���ʾ��	
	*/
	const ASTDecNode* tmp = (const ASTDecNode*)currentP;
	for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
		symTablePtr Entry=nullptr;
		char name[IDNAME_MAX_LEN];
		strcpy(name, tmp->nodeBase.names[i].c_str());
		if (this->FindEntry(name, false, Entry)) {
			throw std::runtime_error(
				strcat(name, " declare twice")
			);
		}
	}

	currentP->tablePtrs.push_back(this->Access);//������ű����
	std::vector<symTablePtr> &table = this->scope.at(this->Level - 1);
	if (tmp->decKind == ASTDecKind::ARRAY_K)
	{
		//�������������Ĵ���

		AttributelR* item = new AttributelR;
		item->idtype = this->arrayType((ASTDecNode*)currentP);
		item->kind = varKind;
		item->VarAttr.access = indir;
		item->VarAttr.level = this->Level;
		item->VarAttr.off = this->scope.at(this->Level - 1).size() == 0 ? 7 :
			this->scope.at(this->Level - 1).back()->attrIR.VarAttr.off + this->scope.at(this->Level - 1).back()->attrIR.idtype->size;

		symTablePtr Entry = nullptr;
		char name[IDNAME_MAX_LEN];
		strcpy(name, tmp->nodeBase.names.back().c_str());
		this->Enter(name, item, Entry);
	}
	else {
		//���������Ĵ���
		if (tmp->decKind == ASTDecKind::INTEGER_K) {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel["intTy"];
				item->kind = varKind;
				item->VarAttr.access = indir;
				item->VarAttr.level = this->Level;
				item->VarAttr.off = this->scope.at(this->Level - 1).size() == 0 ? 7 : 
					this->scope.at(this->Level - 1).back()->attrIR.VarAttr.off + this->scope.at(this->Level - 1).back()->attrIR.idtype->size;

				symTablePtr Entry = nullptr;
				char name[IDNAME_MAX_LEN];
				strcpy(name, tmp->nodeBase.names[i].c_str());
				this->Enter(name, item, Entry);
			}
		}
		else {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel["charTy"];
				item->kind = varKind;
				item->VarAttr.access = indir;
				item->VarAttr.level = this->Level;
				item->VarAttr.off = this->scope.at(this->Level - 1).size() == 0 ? 7 :
					this->scope.at(this->Level - 1).back()->attrIR.VarAttr.off + this->scope.at(this->Level - 1).back()->attrIR.idtype->size;

				symTablePtr Entry = nullptr;
				char name[IDNAME_MAX_LEN];
				strcpy(name, tmp->nodeBase.names[i].c_str());
				this->Enter(name, item, Entry);
			}
		}
	}
}

void Table::TypeDecPart(ASTNodeBase* currentP) {
	/*
		��������Tʱ���������ڲ��ڵ�TPtr;����"idname=T"������ű�
		���鱾�������������Ƿ����ظ��������
	*/
	if (currentP == nullptr)return;
	const ASTDecNode* tmp = (const ASTDecNode*)currentP;

	AttributelR* item = new AttributelR;
	item->idtype = this->TypeProcess(currentP);
	item->kind = typeKind;

	symTablePtr Entry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, tmp->nodeBase.names[0].c_str());
	this->Enter(name, item, Entry);

	//�������е�type����
	this->TypeDecPart(currentP->sibling);
}


void Table::ProcDecPart(ASTNodeBase* currentP) {
	/*
		�ڵ�ǰ����ű�����д���̱�ʶ�������ԣ����²���ű�����д��
		�α�ʶ�������ԡ�
	*/
	const ASTDecNode* tmp = (const ASTDecNode*)currentP;
	for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
		symTablePtr Entry=nullptr;
		char name[IDNAME_MAX_LEN];
		strcpy(name, tmp->nodeBase.names[i].c_str());
		if (this->FindEntry(name, false, Entry)) {
			throw std::runtime_error(
				strcat(name, " declare twice")
			);
		}
	}

	currentP->tablePtrs.push_back(this->Access);//������ű����
	//�����������Ĵ���
	AttributelR* item = new AttributelR;
	item->idtype = nullptr;
	item->kind = procKind;
	item->More.level = this->Level;
	item->More.size = 1;
	item->More.param = nullptr;

	symTablePtr Entry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, currentP->names.back().c_str());
	this->Enter(name, item, Entry);

	//�����ӳ���
	this->CreatTable();
	//���������
	Entry->attrIR.More.param = this->ParaDecList(currentP->child[0]);

}

symTablePtr Table::HeadProcess(ASTNodeBase* currentP) {

	/*
		�ڵ�ǰ����ű�����д������ʶ�������ԣ����²���ű�����д��
		�α�ʶ�������ԡ����й��̵Ĵ�С�ʹ������Ժ���
	*/

	return nullptr;
}

ParamTable* Table::ParaDecList(ASTNodeBase* currentP){
	/*
		���µķ��ű��еǼ������βεı�������βα���ĵ�ַ����
		ʹparamָ��˵�ַ��
	*/
	if (currentP == nullptr) return nullptr;
	this->VarDecList(currentP);//����table

	ParamTable* param = new  ParamTable();
	param->entry = (void*)this->scope.at(this->Level - 1).at(this->scope.at(this->Level - 1).size()-1);
	param->next = ParaDecList(currentP->sibling);

	return param;
}

void Table::Body(ASTNodeBase* currentP) {
	/*
		SNL����ϵͳ��ִ���岿�ּ�Ϊ������У���ֻ�账��������в�
		�֡�
	*/
	ASTNodeBase* tmp = currentP;
	while (tmp != nullptr) {
		this->statement(tmp);
		tmp = tmp->sibling;
	}
}

void Table::statement(ASTNodeBase* currentP) {
	/*
		�����﷨���ڵ��е�kind���ж�Ӧ��ת�����ĸ�������ͺ�����
	*/
	ASTStmtNode* tmp = ( ASTStmtNode*)currentP;
	switch (tmp->stmtKind)
	{
	case ASTStmtKind::ASSIGN_K:
		this->assignstatement(tmp);
		break;
	case ASTStmtKind::IF_K:
		//std::cout << "if:" << std::endl;
		this->ifstatment(tmp);
		//std::cout << "endif" <<std::endl;
		break;
	case ASTStmtKind::WHILE_K:
		//std::cout << "while:" << std::endl;
		this->whilestatement(tmp);
		//std::cout << "endwhile:" << std::endl;
		break;
	case ASTStmtKind::CALL_K :
		this->callstatement(tmp);
		break;
	case ASTStmtKind::READ_K:
		this->readstatemen(currentP);
		break;
	case ASTStmtKind::WRITE_K:
		this->writestatemen(currentP);
		break;
	default:
		break;
	}
}


TypeIR* Table::arrayVar(ASTNodeBase* currentP) {
	/*
		���var:=var0[E]��var0�ǲ����������ͱ�����E�ǲ��Ǻ������
		�±��������ƥ�䡣
	*/
	symTablePtr Entry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, currentP->names[0].c_str());
	if (this->FindEntry(name, true, Entry)) {

		//�ж��Ƿ�Ϊ����
		if (Entry->attrIR.idtype->kind != arrayTy) {
			throw std::runtime_error(
				strcat(name, " id not array")
			);
		}

		//�ж��±�����
		const ASTExpNode* exp = (const ASTExpNode*)currentP->child[0];
		if (exp->expKind != ASTEXPKind::OP_K) {
			//�Ǳ��ʽ����±�
			if (exp->expKind == ASTEXPKind::CONST_K && Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind != intTy) {
				throw std::runtime_error(
					strcat(name, " index type incorrect")
				);
			}
			else if (exp->expKind == ASTEXPKind::ID_K) {
				symTablePtr indexEntry = nullptr;
				char name[IDNAME_MAX_LEN];
				strcpy(name, exp->nodeBase.names[0].c_str());
				if (!this->FindEntry(name, true, indexEntry)) {
					throw std::runtime_error(
						strcat(name, " not declare")
					);
				}
				if (indexEntry->attrIR.kind != Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind) {
					throw std::runtime_error(
						strcat(name, " index type incorrect")
					);
				}
			}
		}
		else {
			//�Ǳ��ʽ���еݹ����
			TypeIR* idtype = this->Expr(currentP->child[0]);
			if (idtype->kind != Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind) {
				throw std::runtime_error(
					strcat(name, " index type incorrect")
				);
			}
		}
	}
	else {
		//��������δ����
		throw std::runtime_error(
			strcat(name, " not declare")
		);
	}
	return Entry->attrIR.idtype;
}

TypeIR* Table::Expr(ASTNodeBase* currentP) {
	/*
		���ʽ����������ص��Ǽ��������������������ԣ�����ʽ��
		���͡����в���Ekind������ʾʵ���Ǳ�λ���ֵ�Ρ�
	*/
	const ASTExpNode* exp = (const ASTExpNode*)currentP;

	if (exp->expKind == ASTEXPKind::OP_K) {
		TypeIR* exlL = this->Expr(currentP->child[0]);
		TypeIR* exlR = this->Expr(currentP->child[1]);

		if (exlL->kind != arrayTy && exlR->kind != arrayTy) {
			if (exlL->kind != exlR->kind) {
				throw std::runtime_error(
					(std::to_string(exp->nodeBase.lineNum) + ": different type can not op").c_str()
				);
			} 
		}
		else if (exlL->kind == arrayTy) {
			if (exlR->kind == arrayTy) {
				if (exlL->More.ArrayAttr.elemTy->kind != exlR->More.ArrayAttr.elemTy->kind) {
					throw std::runtime_error(
						(std::to_string(exp->nodeBase.lineNum) + ": different type can not op").c_str()
					);
				}
			}
			else if (exlL->More.ArrayAttr.elemTy->kind != exlR->kind) {
				throw std::runtime_error(
					(std::to_string(exp->nodeBase.lineNum) + ": different type can not op").c_str()
				);
			}
			
		}
		else {
			if (exlR->More.ArrayAttr.elemTy->kind != exlL->kind) {
				throw std::runtime_error(
					(std::to_string(exp->nodeBase.lineNum) + ": different type can not op").c_str()
				);
			}
		}
		//����ʽ��������  ##tipsbool���Ͳ�������δ����
		if (exp->expAttr.op == ASTOpType::EQ || exp->expAttr.op==ASTOpType::LT) {
			
			TypeIR* typeir = new TypeIR;
			typeir->kind = boolTy;
			return typeir;
		}
		else if (exp->expAttr.op == ASTOpType::MINUS || exp->expAttr.op == ASTOpType::PLUS
			|| exp->expAttr.op == ASTOpType::TIMES || exp->expAttr.op == ASTOpType::OVER) {
			
			TypeIR* typeir = new TypeIR;
			if (exlL->kind == arrayTy) typeir->kind = exlL->More.ArrayAttr.elemTy->kind;
			else typeir->kind = exlL->kind;
			return typeir;
		}
	}
	else if(exp->expKind == ASTEXPKind::ID_K){

		symTablePtr ptr = nullptr;
		char name[IDNAME_MAX_LEN];
		strcpy(name, exp->nodeBase.names[0].c_str());
		//�������
		if (!this->FindEntry(name, true, ptr))
		{
			throw std::runtime_error(
				strcat(name, " not declare")
			);
		}
		
		//����������Ҫ����±�
		if (ptr->attrIR.idtype->kind == arrayTy) this->arrayVar(currentP);
		return ptr->attrIR.idtype;
	}
	else {
		//Ĭ��const�������γ���
		return this->typetabel["intTy"];
	}
}

void Table::assignstatement(ASTStmtNode* currentP) {
	/*
		��ֵ��������������ص��Ǽ�鸳ֵ�����˷��������������ԡ�
		������߷�const ���������ͬ�����һ��λconst��һ������Ϊchar,�������ͼ��elem���Ͳ���Ϊchar
	*/
	symTablePtr Exp1 = nullptr,Exp2 = nullptr;
	const ASTExpNode* tmp1 = (const ASTExpNode*)currentP->nodeBase.child[0];
	const ASTExpNode* tmp2 = (const ASTExpNode*)currentP->nodeBase.child[1];
	char name1[IDNAME_MAX_LEN], name2[IDNAME_MAX_LEN];

	//�����������Ƿ��Ѿ��������������������array��Ҫ����������飬�±������ǹ�ƥ��ȵ�
	if (tmp1->expKind != ASTEXPKind::CONST_K) {
		strcpy(name1, currentP->nodeBase.child[0]->names[0].c_str());
		if (!this->FindEntry(name1, true, Exp1))
			throw std::runtime_error(
				strcat(name1, " not declare")
			);
		if (Exp1->attrIR.idtype->kind == arrayTy) this->arrayVar(currentP->nodeBase.child[0]);
	}
	else if (tmp1->expKind == ASTEXPKind::OP_K) {
		//��ֵ����Ϊ���ʽ
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + " : left can not be a Exp").c_str()
		);
	}
	if (tmp2->expKind != ASTEXPKind::CONST_K && tmp2->expKind != ASTEXPKind::OP_K) {
		strcpy(name2, currentP->nodeBase.child[1]->names[0].c_str());
		if (!this->FindEntry(name2, true, Exp2))		
			throw std::runtime_error(
				strcat(name2, " not declare")
			);
		if (Exp2->attrIR.idtype->kind == arrayTy) this->arrayVar(currentP->nodeBase.child[1]);
	}
	else if (tmp2->expKind == ASTEXPKind::OP_K) {
		//��ֵΪ���ʽ
		Exp2 = new symTable();
		Exp2->attrIR.idtype=this->Expr(currentP->nodeBase.child[1]);
	}


	//���������������Ƿ�ƥ��
	if (tmp1->expKind != ASTEXPKind::CONST_K && tmp2->expKind != ASTEXPKind::CONST_K) {
		TypeKind exp1 = Exp1->attrIR.idtype->kind != arrayTy ? Exp1->attrIR.idtype->kind 
			: Exp1->attrIR.idtype->More.ArrayAttr.elemTy->kind;
		TypeKind exp2 = Exp2->attrIR.idtype->kind != arrayTy ? Exp2->attrIR.idtype->kind 
			: Exp2->attrIR.idtype->More.ArrayAttr.elemTy->kind;
		if (exp1!=exp2) {
			throw std::runtime_error(
				(std::to_string(currentP->nodeBase.lineNum) + ": type unfit").c_str()
			);
		}
	}
	else if (tmp1->expKind == ASTEXPKind::CONST_K && tmp2->expKind == ASTEXPKind::CONST_K) {
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + ": const can not assign").c_str()
		);
	}
	else {
		if (tmp1->expKind == ASTEXPKind::CONST_K) {

			TypeKind exp2 = Exp2->attrIR.idtype->kind != arrayTy ? Exp2->attrIR.idtype->kind 
				: Exp2->attrIR.idtype->More.ArrayAttr.elemTy->kind;
			if (exp2 != intTy) {
				throw std::runtime_error(
					(std::to_string(currentP->nodeBase.lineNum) + ": type unfit").c_str()
				);
			}
		}
		else {
			TypeKind exp1 = Exp1->attrIR.idtype->kind != arrayTy ? Exp1->attrIR.idtype->kind
				: Exp1->attrIR.idtype->More.ArrayAttr.elemTy->kind;
			if (exp1 != intTy) {
				throw std::runtime_error(
					(std::to_string(currentP->nodeBase.lineNum) + ": type unfit").c_str()
				);
			}
		}
	}
}

void Table::ifstatment(ASTStmtNode* currentP) {
	/*
		����������ʽ�Ƿ�Ϊbool���ͣ�����then������в��ֺ�else
		������в��֡�
	*/
	
	TypeIR* item = this->Expr(currentP->nodeBase.child[0]);
	if (item->kind != boolTy) {
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + ": if condition stmt not bool").c_str()
		);
	}

	//����then��else����
	for (int i = 1; i < 3; i++) this->Body(currentP->nodeBase.child[i]);
	
}


void Table::whilestatement(ASTStmtNode* currentP) {
	/*
		����������ʽ�Ƿ�Ϊbool���ͣ�����������в��֡�
	*/

	TypeIR* item = this->Expr(currentP->nodeBase.child[0]);
	if (item->kind != boolTy) {
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + ": while condition stmt not bool").c_str()
		);
	}

	//����whileѭ����
	this->Body(currentP->nodeBase.child[1]);

}

void Table::callstatement(ASTStmtNode* currentP) {
	/*
		����������������������ȼ����ű�����������е�Param��
		��(�βη��ű����ַ��),����������βκ�ʵ��֮��Ķ�Ӧ��ϵ
		�Ƿ���ȷ��
	*/

	const ASTExpNode* proc = (const ASTExpNode*)currentP->nodeBase.child[0];
	const ASTExpNode* param = (const ASTExpNode*)currentP->nodeBase.child[1];

	//��ѯ�������Ƿ����
	symTablePtr procEntry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, proc->nodeBase.names[0].c_str());
	if (!this->FindEntry(name, true,procEntry)) {
		throw std::runtime_error(
			strcat(name, " not declare ")
		);
		
	}

	//��ѯ�����Ƿ���� ������ƥ��
	this->paramstatemnt(procEntry->attrIR.More.param, procEntry->attrIR.More.level, currentP->nodeBase.child[1]);
}

void Table::paramstatemnt(ParamTable* paramItem,int level,ASTNodeBase* currentP) {
	/*
		��ѯ�����Ƿ���� ������ƥ��
	*/
	if (currentP == nullptr) {
		if (paramItem != nullptr)
			throw std::runtime_error(
				(std::to_string(currentP->lineNum) + ": paramters not enough").c_str()
			);
		return;
	}
	if (paramItem == nullptr) 
		throw std::runtime_error(
			(std::to_string(currentP->lineNum) + ": too many param").c_str()
		);
	const ASTExpNode* param = (const ASTExpNode*)currentP;
	symTablePtr paramEntry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, param->nodeBase.names[0].c_str());
	if (this->FindEntry(name, true, paramEntry)) {
		if (((symTablePtr)paramItem->entry)->attrIR.idtype->kind != paramEntry->attrIR.idtype->kind)
			throw std::runtime_error(
				(std::to_string(currentP->lineNum) + ": param  not same").c_str()
			);
	}
	else {
		throw std::runtime_error(
			(std::to_string(currentP->lineNum) + ": param  not declar").c_str()
		);
	}
	this->paramstatemnt(paramItem->next, level, currentP->sibling);
}


void Table::writestatemen(ASTNodeBase* currentP) {
	/*
		����д����еı��ʽ�Ƿ�Ϸ���
	*/

	TypeIR * type=this->Expr(currentP->child[0]);
}

void Table::readstatemen(ASTNodeBase* currentP) {
	/*
		���Ҫ����ı��������������Ƿ�Ϊ������
	*/

	ASTStmtNode* tmp = (ASTStmtNode*)currentP;

	symTablePtr Entry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, tmp->nodeBase.names[0].c_str());
	if (!this->FindEntry(name, true, Entry))
		throw std::runtime_error(
			strcat(name, " not declare")
		);
	if (Entry->attrIR.kind != varKind) {
		throw std::runtime_error(
			(std::to_string(currentP->lineNum) + ": var can not read").c_str()
		);
	}
}

void Table:: analyze(ASTNodeBase* currentP) {
	/*
		��������ܳ��򣬶��﷨�����з�����
	*/
	if (currentP == nullptr) return;

	switch (currentP->nodeKind)
	{
	case ASTNodeKind::PRO_K:
		this->CreatTable();
		for (int i = 0; i < 3; ++i) {
			this->analyze(currentP->child[i]);
		}
		this->DestroyTable();
		this->analyze(currentP->sibling);
		break;
	case ASTNodeKind::PHEAD_K:

		break;
	case ASTNodeKind::TYPE_K:
		//�����ӽڵ���������ֵܽ��ΪVAR_K
		this->TypeDecPart(currentP->child[0]);
		this->analyze(currentP->sibling);
		break;
	case ASTNodeKind::VAR_K:
		//��������������ӽڵ�ΪDEC_K
		this->analyze(currentP->child[0]);
		this->analyze(currentP->sibling);
		break;
	case ASTNodeKind::PROC_K:
		//�����������
		this->analyze(currentP->child[0]);
		break;
	case ASTNodeKind::STM_L_K:
		this->Body(currentP->child[0]);
		break;
	case ASTNodeKind::DEC_K:
		if (((const ASTDecNode*)currentP)->decKind == ASTDecKind::PROC_DEC_K) {
			//�������
			this->ProcDecPart(currentP);
			for (int i = 1; i < 3; i++) {
				this->analyze(currentP->child[i]);
			}
			this->analyze(currentP->sibling);
			//�ӹ��̻���
			this->DestroyTable();
		}
		else{
			//�����������
			this->VarDecList(currentP);
			this->analyze(currentP->sibling);
		}
		break;
	default:
		break;
	}
}

void Table::PrintSymbTabl() {


	for (int i = 0; i < this->tables.size(); i++) {
		this->PrintHead();
		std::vector<symTablePtr> &table = this->tables.at(i);
		for (int j = 0; j < table.size(); j++) {
			std::cout << std::endl << "����������������������������������������������" << std::endl;
			if (table.at(j)->attrIR.kind == varKind) this->PrintVarDecSym(table.at(j));
			else if (table.at(j)->attrIR.kind == procKind) this->PrintProcDecSym(table.at(j));
		}
		std::cout << std::endl << "����������������������������������������������" << std::endl;
	}
}
void Table::PrintHead() {

	std::cout << std::endl << "***********************************************" << std::endl;
	std::cout << "|" << "IDNAME" << "\t|" << "IDKIND" << "\t|" << "TYPEKIND" << "|" <<
		"ACCESS" << "|" << "LEVEL" << "\t|" << "OFFSET" ;
	std::cout << std::endl << "***********************************************" ;
}
void Table::PrintProcDecSym(symTablePtr& Entry){

	std::cout << "|" << Entry->idname << "\t|" << praseIdKind(Entry->attrIR.kind) << "\t|" << 
		"\t|" << Entry->attrIR.More.level << "\t|" << Entry->attrIR.More.size<< std::endl;
	ParamTable* param = Entry->attrIR.More.param;
	while (param != nullptr) {
		std::cout << std::endl << " params:" << praseTypeKind(((symTablePtr)param->entry)->attrIR.idtype->kind)
			<< "\t" << ((symTablePtr)param->entry)->idname;
		param = param->next;
	}
}

void Table::PrintVarDecSym(symTablePtr& Entry) {
	if (Entry->attrIR.idtype->kind == arrayTy) {
		std::cout << "|" << Entry->idname << "\t|" << praseIdKind(Entry->attrIR.kind) << "\t|" <<praseTypeKind(Entry->attrIR.idtype->kind)<<
			"\t|" << praseAccess(Entry->attrIR.VarAttr.access)<< "\t|" << Entry->attrIR.VarAttr.level << "\t|" << Entry->attrIR.VarAttr.off<<
			std::endl << std::endl <<" param: " <<praseTypeKind(Entry->attrIR.idtype->More.ArrayAttr.elemTy->kind) << 
			"\t index: "<< praseTypeKind(Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind);
	}
	else {
		std::cout << "|" << Entry->idname << "\t|" << praseIdKind(Entry->attrIR.kind) << "\t|" 
			<< praseTypeKind(Entry->attrIR.idtype->kind)<<"\t|"<<praseAccess(Entry->attrIR.VarAttr.access)
			<<"\t|"<<Entry->attrIR.VarAttr.level<<"\t|"<<Entry->attrIR.VarAttr.off;
	}
}