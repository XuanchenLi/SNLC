#include "table.h"


Table::~Table() {


}
Table::CreatTable() {
	/*
		当进入一个新的局部化单位时，调用本子程序。功能是建立一个空
		符号表table,层数加1,偏移初始化为0。
	*/
	this->Level++;
	this->scope.push_back(new std::vector<symTablePtr>);
}

Table::DestroyTable() {
	/*
		撤销一个符号表
	*/
	this->Level--;
	//销毁符号表
	this->scope.pop_back();
}

bool Table::Enter(char* Id, AttributelR* AttribP, symTablePtr Entry) {
	/*
		将标识符名，标识符属性登记在符号表中，登记成功，返回值为
		真，Entry指向登记的标识符在符号表中的位置；登记不成功，返回
		值为假，Entry值为空。
	*/
	if (FindEntry(Id, true, Entry)) {
		//多次声明错误
		std::cout << Id << " 重复声明" << endl;
		return false;
	}

	symTablePtr item = new struct symtabe;
	strcpy(item->idname, Id);
	item->next = nullptr;
	item->attrIR = AttribP; //浅拷贝
	

	std::vector<symTablePtr> table=this->scope.at(this->Level - 1);
	table.push_back(item);

	Entry = item;
	return true;
}

bool Table::FindEntry(char* id, bool diraction, symTablePtr Entry) {
	/*
		direction为真是向底部查找，为假向顶部查找
		根据flag的值为one还是total,决定在当前符号表中查找标识符，
		还是在scope栈中的所有符号表中查找标识符。
	*/
	if (diraction) {
		//为真向底部查找
		for (int level = this->scope.size() - 1; level >= 0; level--) {
			if (SearchoneTable(id, level, Entry)) return true;
		}
	}
	else {
		//为假向顶部查找
		for (int level = 0; level < this->scope.size(); level++) {
			if (SearchoneTable(id, level, Entry)) return true;
		}
	}
	Entry = nullptr;
	return false;
}

bool Table::SearchoneTable(char* id, int currentLevel, symTablePtr Entry) {
	/*
		从表头开始，依次将节点中的标识符名字和id比较是否相同，直
		到找到此标识符或到达表尾，若找到，返回真值，Entry为标识符
		在符号表中的位置，否则，返回值为假。
	*/
	std::vector<symTablePtr> table = this->scope.at(currentLevel);

	for (int i = 0; i < table.size(); i++) {
		if (strcmp(table.at(i)->idname, id) == 0) {
			Entry = table.at(i);
			return true;
		}
	}
	Entry = nullptr;
	return false;
}


void Table::initialize() {
	/*
		初始化整数类型，字符类型，布尔类型的内部表示说明由于这三种
		类型均为基本类型，内部表示固定。
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
		类型分析处理。处理语法树的当前节点类型，构造当前类型的内部
		表示，并将其地址返回给Ptr类型内部表示的地址。
	*/
	const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
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
		创建数组类型的内部表示。此时需检查下标是否越界
	*/
	if (currentP->decAttr.arrayAttr.up < currentP->decAttr.arrayAttr.low) {
		std::cout << "over boundary" << endl;
		return nullptr;
	}

	typeIR* item = new typeIR;
	item->kind = arrayTy;
	item->size = currentP->decAttr.arrayAttr.up - currentP->decAttr.arrayAttr.low + 1;
	if (currentP->decAttr.arrayAttr.itemType == ASTDecKind::INTEGER_K) item->More.ArrayAttr.elemTy = this->typetabel.find("intTy")->second;
	else item->More.ArrayAttr.elemTy = this->typetabel.find("charTy")->second;
	//下标类型默认为整形
	item->More.ArrayAttr.indexTy= this->typetabel.find("intTy")->second;
	//加入内部类型表中
	this->typetabel.insert(std::pair<std::string, TypeIR*>(currentP->nodeBase.names.back(), item));
	return item;
}


void Table::VarDecList(ASTNodeBase* currentP) {
	/*
		当遇到变量标识符id时，把id登记到符号表中；检查重复性定义；
		遇到类型时，构造其内部表示。	
	*/
	const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
	for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
		if (this->FindEntry(tmp->nodeBase.names.at(i), true, symTablePtr * Entry)) {
			std::cout << "re declare" << endl;
			return;
		}
	}

	std::vector<symTablePtr> table = this->scope.back();

	if (tmp->decKind != ASTDecKind::ARRAY_K)
	{
		//数组类型声明的处理

		AttributelR* item = new AttributelR;
		item->idtype = this->ProcDecPart(currentP);
		item->kind = arrayTy;
		item->VarAttr.access = indir;
		item->VarAttr.level = this->Level;
		item->VarAttr.off = table.size();

		symTablePtr newsym = new struct symTable;
		strcpy(newsym->idname, tmp->nodeBase.names.back());
		newsym->attrIR = item;
		table.push_back(newsym);
	}
	else {
		//变量声明的处理
		if (tmp->decKind == ASTDecKind::INTEGER_K) {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel.find("intTy")->second;
				item->kind = varKind;
				this->Enter(tmp->nodeBase.names.at(i), item, symTablePtr * Entry);
			}
		}
		else {
			for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
				AttributelR* item = new AttributelR;
				item->idtype = this->typetabel.find("charTy")->second;
				item->kind = varKind;
				this->Enter(tmp->nodeBase.names.at(i), item, symTablePtr * Entry);
			}
		}
	}
}

void Table::ProcDecPart(ASTNodeBase* currentP) {
	/*
		在当前层符号表中填写过程标识符的属性；在新层符号表中填写形
		参标识符的属性。
	*/
	const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
	for (int i = 0; i < tmp->nodeBase.names.size(); ++i) {
		if (this->FindEntry(tmp->nodeBase.names.at(i), true, symTablePtr * Entry)) {
			std::cout << "re declare" << endl;
			return;
		}
	}

	std::vector<symTablePtr> table = this->scope.back();
	//过程名声明的处理
	AttributelR* item = new AttributelR;
	item->idtype = nullptr;
	item->kind = procKind;
	item->More.level = this->Level;
	item->More = 0;
	item->More.size = 1;
	item->More.param = nullptr;

	symTablePtr newsym = new struct symTable;
	strcpy(newsym->idname, currentP->names.back());
	table.push_back(newsym);

	//进入子程序
	this->CreatTable();
	//回填函数参数
	item->More.param=this->ParaDecList(currentP->child[0])

}

symTablePtr Table::HeadProcess(ASTNodeBase* currentP) {

	/*
		在当前层符号表中填写函数标识符的属性；在新层符号表中填写形
		参标识符的属性。其中过程的大小和代码需以后回填。
	*/


}

ParamTable* Table::ParaDecList(ASTNodeBase* currentP){
	/*
		在新的符号表中登记所有形参的表项，构造形参表项的地址表，并
		使param指向此地址表。
	*/
	if (currentP == nullptr) return nullptr;

	const ASTDecNode* tmp = (const ASTDecNode*)&currentP;

	ParamTable* param = new struct ParamTable;
	param->entry = this->scope.back().size();
	param->next = ParaDecList(currentP->sibling);

	return param;
}

void Table::Body(ASTNodeBase* currentP) {
	/*
		SNL编译系统的执行体部分即为语句序列，故只需处理语句序列部
		分。
	*/
	const ASTStmtNode* tmp = (const ASTStmtNode*)&currentP;
	while (tmp != nullptr) {
		this->statement(tmp);
	}
}

void Table::statement(ASTStmtNode*& currentP) {
	/*
		根据语法树节点中的kind项判断应该转向处理哪个语句类型函数。
	*/
	switch (currentP->stmtKind)
	{
	case ASTStmtKind::ASSIGN_K:
		assignstatement(currentP);
		break;
	case:ASTStmtKind::IF_K :
		ifstatment(currentP);
		break;
	case ASTStmtKind::WHILE_K:

		break;
	case ASTStmtKind::CALL_K :

		break;
	default:
		break;
	}
}

void Table::assignstatement(ASTStmtNode* currentP) {
	/*
		赋值语句的语义分析的重点是检查赋值号两端分量的类型相容性。
		如果两边非const 检查类型相同，如果一方位const另一方不能为char,数组类型检查elem类型不必为char
	*/
	symTablePtr Exp1,Exp2;
	const ASTExpNode* tmp1 = (const ASTExpNode*)&currentP->nodeBase.child[0];
	const ASTExpNode* tmp2 = (const ASTExpNode*)&currentP->nodeBase.child[1];
	if (this->FindEntry(currentP->nodeBase.child[0]->names.back(), true, Exp1) && this->FindEntry(currentP->nodeBase.child[1]->names.back(), true, Exp2)) {
		if (tmp1->expKind != ASTEXPKind::CONST_K && tmp2->expKind != ASTEXPKind::CONST_K) {
			if (Exp1->attrIR.idtype->kind != Exp2->attrIR.idtype->kind) {
				std::cout << "type unfit" << endl;
				return;
			}
		}
		else if (tmp1->expKind == ASTEXPKind::CONST_K && tmp2->expKind == ASTEXPKind::CONST_K) {
			std::cout << "const cant assign" << endl;
			return;
		}
		else {
			if (tmp1->expKind == ASTEXPKind::CONST_K) {
				if (Exp2->attrIR.idtype->kind == arrayTy&&Exp2->attrIR.idtype->More.ArrayAttr.elemTy->kind==charTy) {
					std::cout << "array elemtype unfit" << endl;
					return;
				}
				else if (Exp2->attrIR.idtype->kind == charTy) {
					std::cout << "type unfit" << endl;
					return;
				}
			}
			else {
				if (Exp1->attrIR.idtype->kind == charTy) {
					std::cout << "type unfit" << endl;
					return;
				}
			}
		}

	}
}

void Table::ifstatment(ASTStmtNode* currentP) {


}

void Table:: Analyze(ASTNodeBase* currentP) {
	/*
		语义分析总程序，对语法树进行分析。
	*/
	switch (currentP->nodeKind)
	{

	case ASTNodeKind::PRO_K:
		for (int i = 0; i < 3; ++i) {
			this->Analyze(currentP->child[i]);
		}
		break;
	case ASTNodeKind::PHEAD_K:
		break;
	case ASTNodeKind::TYPE_K:
		//处理子节点的声明，兄弟结点为VAR_K
		this->Analyze(currentP->child[0]);
		this->Analyze(currentP->sibling);
		break;
	case ASTNodeKind::VAR_K:
		//处理变量声明，子节点为DEC_K
		this->Analyze(currentP->child[0]);
		this->Analyze(currentP->sibling);
		break;
	case ASTNodeKind::PROC_K:
		//处理过程声明
		this->Analyze(currentP->child[0]);
		break;
	case ASTNodeKind::STM_L_K:
		this->Body(currentP->child[0]);
		break;
	case ASTNodeKind::DEC_K:
		const ASTDecNode* tmp = (const ASTDecNode*)&currentP;
		if (tmp->decKind == ASTDecKind::PROC_DEC_K) {
			//处理过程
			this->ProcDecPart(currentP);
			for (int i = 1; i < 3; i++) {
				this->Analyze(currentP->child[i]);
			}
			this->Analyze(currentP->sibling);
			//子过程回退
			this->DestroyTable();
		}
		else{
			//处理变量声明
			for (int i = 0; i < 3; ++i) {
				this->Analyze(currentP->child[i]);
			}
			this->Analyze(currentP->sibling);
		}
		break;
	default:
		break;
	}


}