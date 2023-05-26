#include "table.h"


Table::~Table() {


}
void Table::CreatTable() {
	/*
		当进入一个新的局部化单位时，调用本子程序。功能是建立一个空
		符号表table,层数加1,偏移初始化为0。
	*/
	this->Level++;
	std::vector<symTablePtr> newitem;
	this->scope.push_back(newitem);
}

void Table::DestroyTable() {
	/*
		撤销一个符号表
		从分程序表中弹出，加入符号表集合
	*/
	this->Access++;
	this->tables.push_back(this->scope.back());

	this->Level--;
	this->scope.pop_back();

}

bool Table::Enter(char* Id, AttributelR* AttribP, symTablePtr &Entry) {
	/*
		将标识符名，标识符属性登记在符号表中，登记成功，返回值为
		真，Entry指向登记的标识符在符号表中的位置；登记不成功，返回
		值为假，Entry值为空。
	*/
	if (FindEntry(Id, false , Entry)) {
		//多次声明错误
		throw std::runtime_error(
			strcat(Id, " declare twice")
		);
	}

	symTablePtr item = new symTable();
	strcpy(item->idname, Id);
	item->next = nullptr;
	item->attrIR = *AttribP; //浅拷贝
	
	this->scope.at(this->Level - 1).push_back(item);

	Entry = item;
	return true;
}

bool Table::FindEntry(char* id, bool diraction, symTablePtr &Entry) {
	/*
		direction为真是向底部查找，为假在本层内查找
		根据flag的值为one还是total,决定在当前符号表中查找标识符，
		还是在scope栈中的所有符号表中查找标识符。
	*/
	if (diraction) {
		//为真向底部查找
		for (int level = this->Level-1; level >= 0; level--) {
			if (SearchoneTable(id, level, Entry)) return true;
		}
	}
	else {
		//为假本层内查找
		if (SearchoneTable(id, this->Level - 1, Entry)) return true;
	}
	Entry = nullptr;
	return false;
}

bool Table::SearchoneTable(char* id, int currentLevel, symTablePtr &Entry) {
	/*
		从表头开始，依次将节点中的标识符名字和id比较是否相同，直
		到找到此标识符或到达表尾，若找到，返回真值，Entry为标识符
		在符号表中的位置，否则，返回值为假。
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
		创建数组类型的内部表示。此时需检查下标是否越界
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
	//下标类型默认为整形
	item->More.ArrayAttr.indexTy= this->typetabel["intTy"];
	//加入内部类型表中
	this->typetabel.insert(std::pair<std::string, TypeIR*>(currentP->nodeBase.names.back(), item));
	return item;
}


void Table::VarDecList(ASTNodeBase* currentP) {
	/*
		当遇到变量标识符id时，把id登记到符号表中；检查重复性定义；
		遇到类型时，构造其内部表示。	
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

	currentP->tablePtrs.push_back(this->Access);//回填符号表入口
	std::vector<symTablePtr> &table = this->scope.at(this->Level - 1);
	if (tmp->decKind == ASTDecKind::ARRAY_K)
	{
		//数组类型声明的处理

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
		//变量声明的处理
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
		遇到类型T时，构造其内部节点TPtr;对于"idname=T"构造符号表
		项；检查本层类型声明中是否有重复定义错误。
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

	//处理所有的type定义
	this->TypeDecPart(currentP->sibling);
}


void Table::ProcDecPart(ASTNodeBase* currentP) {
	/*
		在当前层符号表中填写过程标识符的属性；在新层符号表中填写形
		参标识符的属性。
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

	currentP->tablePtrs.push_back(this->Access);//回填符号表入口
	//过程名声明的处理
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

	//进入子程序
	this->CreatTable();
	//回填函数参数
	Entry->attrIR.More.param = this->ParaDecList(currentP->child[0]);

}

symTablePtr Table::HeadProcess(ASTNodeBase* currentP) {

	/*
		在当前层符号表中填写函数标识符的属性；在新层符号表中填写形
		参标识符的属性。其中过程的大小和代码需以后回填。
	*/

	return nullptr;
}

ParamTable* Table::ParaDecList(ASTNodeBase* currentP){
	/*
		在新的符号表中登记所有形参的表项，构造形参表项的地址表，并
		使param指向此地址表。
	*/
	if (currentP == nullptr) return nullptr;
	this->VarDecList(currentP);//加入table

	ParamTable* param = new  ParamTable();
	param->entry = (void*)this->scope.at(this->Level - 1).at(this->scope.at(this->Level - 1).size()-1);
	param->next = ParaDecList(currentP->sibling);

	return param;
}

void Table::Body(ASTNodeBase* currentP) {
	/*
		SNL编译系统的执行体部分即为语句序列，故只需处理语句序列部
		分。
	*/
	ASTNodeBase* tmp = currentP;
	while (tmp != nullptr) {
		this->statement(tmp);
		tmp = tmp->sibling;
	}
}

void Table::statement(ASTNodeBase* currentP) {
	/*
		根据语法树节点中的kind项判断应该转向处理哪个语句类型函数。
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
	
	default:
		break;
	}
}


TypeIR* Table::arrayVar(ASTNodeBase* currentP) {
	/*
		检查var:=var0[E]中var0是不是数组类型变量，E是不是和数组的
		下标变量类型匹配。
	*/
	symTablePtr Entry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, currentP->names[0].c_str());
	if (this->FindEntry(name, true, Entry)) {

		//判断是否为数组
		if (Entry->attrIR.idtype->kind != arrayTy) {
			throw std::runtime_error(
				strcat(name, " id not array")
			);
		}

		//判断下标类型
		const ASTExpNode* exp = (const ASTExpNode*)currentP->child[0];
		if (exp->expKind != ASTEXPKind::OP_K) {
			//非表达式检测下标
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
			//是表达式进行递归操作
			TypeIR* idtype = this->Expr(currentP->child[0]);
			if (idtype->kind != Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind) {
				throw std::runtime_error(
					strcat(name, " index type incorrect")
				);
			}
		}
	}
	else {
		//数组类型未声明
		throw std::runtime_error(
			strcat(name, " not declare")
		);
	}
	return Entry->attrIR.idtype;
}

TypeIR* Table::Expr(ASTNodeBase* currentP) {
	/*
		表达式语义分析的重点是检查运算分量的类型相容性，求表达式的
		类型。其中参数Ekind用来表示实参是变参还是值参。
	*/
	const ASTExpNode* exp = (const ASTExpNode*)currentP;

	if (exp->expKind == ASTEXPKind::OP_K) {
		TypeIR* exlL = this->Expr(currentP->child[0]);
		TypeIR* exlR = this->Expr(currentP->child[1]);

		if (exlL->kind != arrayTy && exlR->kind != arrayTy) {
			if (exlL->kind != exlR->kind) {
				throw std::runtime_error(
					(std::to_string(exp->nodeBase.lineNum) + "different type can not op").c_str()
				);
			} 
		}
		else if (exlL->kind == arrayTy) {
			if (exlR->kind == arrayTy) {
				if (exlL->More.ArrayAttr.elemTy->kind != exlR->More.ArrayAttr.elemTy->kind) {
					throw std::runtime_error(
						(std::to_string(exp->nodeBase.lineNum) + "different type can not op").c_str()
					);
				}
			}
			else if (exlL->More.ArrayAttr.elemTy->kind != exlR->kind) {
				throw std::runtime_error(
					(std::to_string(exp->nodeBase.lineNum) + "different type can not op").c_str()
				);
			}
			
		}
		else {
			if (exlR->More.ArrayAttr.elemTy->kind != exlL->kind) {
				throw std::runtime_error(
					(std::to_string(exp->nodeBase.lineNum) + "different type can not op").c_str()
				);
			}
		}
		//求表达式计算类型  ##tipsbool类型不可运算未考虑
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
		//声明检查
		if (!this->FindEntry(name, true, ptr))
		{
			throw std::runtime_error(
				strcat(name, " not declare")
			);
		}
		
		//数组类型需要检测下标
		if (ptr->attrIR.idtype->kind == arrayTy) this->arrayVar(currentP);
		return ptr->attrIR.idtype;
	}
	else {
		//默认const都是整形常数
		return this->typetabel["intTy"];
	}
}

void Table::assignstatement(ASTStmtNode* currentP) {
	/*
		赋值语句的语义分析的重点是检查赋值号两端分量的类型相容性。
		如果两边非const 检查类型相同，如果一方位const另一方不能为char,数组类型检查elem类型不必为char
	*/
	symTablePtr Exp1 = nullptr,Exp2 = nullptr;
	const ASTExpNode* tmp1 = (const ASTExpNode*)currentP->nodeBase.child[0];
	const ASTExpNode* tmp2 = (const ASTExpNode*)currentP->nodeBase.child[1];
	char name1[IDNAME_MAX_LEN], name2[IDNAME_MAX_LEN];

	//如果变量检查是否已经声明，如果变量类型是array需要进行数组检验，下标类型是够匹配等等
	if (tmp1->expKind != ASTEXPKind::CONST_K) {
		strcpy(name1, currentP->nodeBase.child[0]->names[0].c_str());
		if (!this->FindEntry(name1, true, Exp1))
			throw std::runtime_error(
				strcat(name1, " not declare")
			);
		if (Exp1->attrIR.idtype->kind == arrayTy) this->arrayVar(currentP->nodeBase.child[0]);
	}
	else if (tmp1->expKind == ASTEXPKind::OP_K) {
		//左值不能为表达式
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + " left can not be a Exp").c_str()
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
		//右值为表达式
		Exp2 = new symTable();
		Exp2->attrIR.idtype=this->Expr(currentP->nodeBase.child[1]);
	}


	//检验运算数类型是否匹配
	if (tmp1->expKind != ASTEXPKind::CONST_K && tmp2->expKind != ASTEXPKind::CONST_K) {
		TypeKind exp1 = Exp1->attrIR.idtype->kind != arrayTy ? Exp1->attrIR.idtype->kind 
			: Exp1->attrIR.idtype->More.ArrayAttr.elemTy->kind;
		TypeKind exp2 = Exp2->attrIR.idtype->kind != arrayTy ? Exp2->attrIR.idtype->kind 
			: Exp2->attrIR.idtype->More.ArrayAttr.elemTy->kind;
		if (exp1!=exp2) {
			throw std::runtime_error(
				(std::to_string(currentP->nodeBase.lineNum) + "type unfit").c_str()
			);
		}
	}
	else if (tmp1->expKind == ASTEXPKind::CONST_K && tmp2->expKind == ASTEXPKind::CONST_K) {
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + "const can not assign").c_str()
		);
	}
	else {
		if (tmp1->expKind == ASTEXPKind::CONST_K) {

			TypeKind exp2 = Exp2->attrIR.idtype->kind != arrayTy ? Exp2->attrIR.idtype->kind 
				: Exp2->attrIR.idtype->More.ArrayAttr.elemTy->kind;
			if (exp2 != intTy) {
				throw std::runtime_error(
					(std::to_string(currentP->nodeBase.lineNum) + "type unfit").c_str()
				);
			}
		}
		else {
			TypeKind exp1 = Exp1->attrIR.idtype->kind != arrayTy ? Exp1->attrIR.idtype->kind
				: Exp1->attrIR.idtype->More.ArrayAttr.elemTy->kind;
			if (exp1 != intTy) {
				throw std::runtime_error(
					(std::to_string(currentP->nodeBase.lineNum) + "type unfit").c_str()
				);
			}
		}
	}
}

void Table::ifstatment(ASTStmtNode* currentP) {
	/*
		检查条件表达式是否为bool类型，处理then语句序列部分和else
		语句序列部分。
	*/
	
	TypeIR* item = this->Expr(currentP->nodeBase.child[0]);
	if (item->kind != boolTy) {
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + " if condition stmt not bool").c_str()
		);
	}

	//处理then和else部分
	for (int i = 1; i < 3; i++) this->Body(currentP->nodeBase.child[i]);
	
}


void Table::whilestatement(ASTStmtNode* currentP) {
	/*
		检查条件表达式是否为bool类型，处理语句序列部分。
	*/

	TypeIR* item = this->Expr(currentP->nodeBase.child[0]);
	if (item->kind != boolTy) {
		throw std::runtime_error(
			(std::to_string(currentP->nodeBase.lineNum) + "while condition stmt not bool").c_str()
		);
	}

	//处理while循环体
	this->Body(currentP->nodeBase.child[1]);

}

void Table::callstatement(ASTStmtNode* currentP) {
	/*
		函数调用语句的语义分析首先检查符号表求出其属性中的Param部
		分(形参符号表项地址表),并用它检查形参和实参之间的对应关系
		是否正确。
	*/

	const ASTExpNode* proc = (const ASTExpNode*)currentP->nodeBase.child[0];
	const ASTExpNode* param = (const ASTExpNode*)currentP->nodeBase.child[1];

	//差询过程名是否存在
	symTablePtr procEntry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, proc->nodeBase.names[0].c_str());
	if (!this->FindEntry(name, true,procEntry)) {
		throw std::runtime_error(
			strcat(name, " not declare ")
		);
		
	}

	//查询参数是否存在 并进行匹配
	this->paramstatemnt(procEntry->attrIR.More.param, procEntry->attrIR.More.level, currentP->nodeBase.child[1]);
	
	return;
}

void Table::paramstatemnt(ParamTable* paramItem,int level,ASTNodeBase* currentP) {
	/*
		查询参数是否存在 并进行匹配
	*/
	if (currentP == nullptr) {
		if (paramItem != nullptr)
			throw std::runtime_error(
				(std::to_string(currentP->lineNum) + "paramters not enough").c_str()
			);
		
	}
	if (paramItem == nullptr) 
		throw std::runtime_error(
			(std::to_string(currentP->lineNum) + " too many param").c_str()
		);
	const ASTExpNode* param = (const ASTExpNode*)currentP;
	symTablePtr paramEntry = nullptr;
	char name[IDNAME_MAX_LEN];
	strcpy(name, param->nodeBase.names[0].c_str());
	if (this->FindEntry(name, true, paramEntry)) {
		if (((symTablePtr)paramItem->entry)->attrIR.idtype->kind != paramEntry->attrIR.idtype->kind)
			throw std::runtime_error(
				(std::to_string(currentP->lineNum) + " param  not same").c_str()
			);
	}
	else {
		throw std::runtime_error(
			(std::to_string(currentP->lineNum) + " param  not declar").c_str()
		);
	}
	this->paramstatemnt(paramItem->next, level, currentP->sibling);
}


void Table::writestatemen(ASTNodeBase* currentP) {
	/*
		检查要读入的变量有无声明和是否为变量。
	*/
}

void Table::readstatemen(ASTNodeBase* currentP) {
	/*
		分析写语句中的表达式是否合法。
	*/

}

void Table:: analyze(ASTNodeBase* currentP) {
	/*
		语义分析总程序，对语法树进行分析。
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
		//处理子节点的声明，兄弟结点为VAR_K
		this->TypeDecPart(currentP->child[0]);
		this->analyze(currentP->sibling);
		break;
	case ASTNodeKind::VAR_K:
		//处理变量声明，子节点为DEC_K
		this->analyze(currentP->child[0]);
		this->analyze(currentP->sibling);
		break;
	case ASTNodeKind::PROC_K:
		//处理过程声明
		this->analyze(currentP->child[0]);
		break;
	case ASTNodeKind::STM_L_K:
		this->Body(currentP->child[0]);
		break;
	case ASTNodeKind::DEC_K:
		if (((const ASTDecNode*)currentP)->decKind == ASTDecKind::PROC_DEC_K) {
			//处理过程
			this->ProcDecPart(currentP);
			for (int i = 1; i < 3; i++) {
				this->analyze(currentP->child[i]);
			}
			this->analyze(currentP->sibling);
			//子过程回退
			this->DestroyTable();
		}
		else{
			//处理变量声明
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
			std::cout << std::endl << "———————————————————————" << std::endl;
			if (table.at(j)->attrIR.kind == varKind) this->PrintVarDecSym(table.at(j));
			else if (table.at(j)->attrIR.kind == procKind) this->PrintProcDecSym(table.at(j));
		}
		std::cout << std::endl << "———————————————————————" << std::endl;
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
			"\t|" << praseAccess(Entry->attrIR.VarAttr.access)<< "\t|" << Entry->attrIR.VarAttr.level << "\t|" << Entry->attrIR.VarAttr.off<<std::endl << std::endl <<
			" param: " <<praseTypeKind(Entry->attrIR.idtype->More.ArrayAttr.elemTy->kind) << 
			"\t index: "<< praseTypeKind(Entry->attrIR.idtype->More.ArrayAttr.indexTy->kind);
	}
	else {
		std::cout << "|" << Entry->idname << "\t|" << praseIdKind(Entry->attrIR.kind) << "\t|" 
			<< praseTypeKind(Entry->attrIR.idtype->kind)<<"\t|"<<praseAccess(Entry->attrIR.VarAttr.access)
			<<"\t|"<<Entry->attrIR.VarAttr.level<<"\t|"<<Entry->attrIR.VarAttr.off;
	}
}