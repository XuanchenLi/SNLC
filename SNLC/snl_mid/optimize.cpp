#include"optimize.h"

void Optimizer::ConstOptimize() {
	
	
	while (this->point < table.size()) {
		//std::cout << this->point << endl;
		this->DivBaseBlock();
		this->OptiBlock();
		this->point++;
	}

}

void Optimizer::OptiBlock() {

	if (this->table.at(this->point).codekind.compare("ASSIGN") == 0) {
		//对于赋值语句，替换第一个ARG结构；
		this->searchBlock(this->table.at(this->point).Arg1);

		if (this->table.at(this->point).Arg1->form.compare("ValueForm") == 0) {
			//常数加入
			for (int i = 0; i < this->blocks.back().size(); i++) {
				if (this->blocks.back().at(i)->name == this->table.at(this->point).Arg2->name) {
					this->blocks.back().at(i)->value = this->table.at(this->point).Arg1->value;
					return;
				}
			}

			Twotuple* tuple = new Twotuple();
			tuple->name = this->table.at(this->point).Arg2->name;//将被赋值变量加入
			tuple->value = this->table.at(this->point).Arg1->value;//写入常数值
			this->blocks.back().push_back(tuple);
		}
		else {
			//非常弹出
			vector<Twotuple*>::iterator it = this->blocks.back().begin();
			for (int i = 0; i < this->blocks.back().size(); i++,it++) {
				if (this->table.at(this->point).Arg2 != nullptr) {
					if (this->blocks.back().at(i)->name == this->table.at(this->point).Arg2->name) {
						this->blocks.back().erase(it);
						return;
					}
				}
				else {
					if (this->blocks.back().at(i)->name == this->table.at(this->point).res->name) {
						this->blocks.back().erase(it);
						return;
					}
				}

			}
		}
	}
	else if (this->table.at(this->point).codekind.compare("+") == 0 ||
		this->table.at(this->point).codekind.compare("-") == 0 ||
		this->table.at(this->point).codekind.compare("*") == 0 ||
		this->table.at(this->point).codekind.compare("/") == 0 ||
		this->table.at(this->point).codekind.compare("<") == 0 ||
		this->table.at(this->point).codekind.compare(">") == 0) {

		//先进行替换
		this->searchBlock(this->table.at(this->point).Arg1);
		this->searchBlock(this->table.at(this->point).Arg2);

		if (this->table.at(this->point).Arg1->form.compare("ValueForm")==0 && this->table.at(this->point).Arg2->form.compare("ValueForm")==0) {
			this->addToBlock(this->table.at(this->point).res->name,
				this->table.at(this->point).Arg1->value + this->table.at(this->point).Arg2->value);
			vector<Argnode>::iterator it = this->table.begin();
			it += this->point;
			this->table.erase(it);
			this->point--;//和循环固定的++抵消，防止跳过
		}
	}
	else if (this->table.at(this->point).codekind.compare("WRITE")==0) {
		//条件跳转语句、输出语句，替换第一个ARG结构
		this->searchBlock(this->table.at(this->point).Arg1);
	}
	else if (this->table.at(this->point).codekind.compare("AADD") == 0) {
		//地址加运算
			
	}
}

bool Optimizer::addToBlock(string name, int val) {

	Twotuple* tuple = new Twotuple();
	tuple->name = name;//将被赋值变量加入
	tuple->value = val;//写入常数值
	this->blocks.back().push_back(tuple);
	return true;
}

bool Optimizer::searchBlock(ArgRecord*& arg) {

	for (int i = 0; i < this->blocks.back().size(); i++) {
		if (this->blocks.back().at(i)->name == arg->name) {
			arg->value=this->blocks.back().at(i)->value;
			arg->form = "ValueForm";
			arg->name = std::to_string(this->blocks.back().at(i)->value);
			return true;
		}
	}
	return false;
}

bool Optimizer::DivBaseBlock(){

	if (this->table.at(this->point).codekind.compare("ENTRY") == 0|| 
		this->table.at(this->point).codekind.compare("WHILE") == 0|| 
		this->table.at(this->point).codekind.compare("THEN") == 0||
		this->table.at(this->point).codekind.compare("ELSE") == 0) {

		//进入一个新的块
		this->point++;
		std::vector<Twotuple*> block;
		this->blocks.push_back(block);
		return true;
	}
	else if (this->table.at(this->point).codekind.compare("ENDPROC") == 0 ||
		this->table.at(this->point).codekind.compare("ENDWHILE") == 0 ||
		this->table.at(this->point).codekind.compare("ENDIF") == 0
		) 
	{
		//结束一个块
		this->point++;
		this->blocks.pop_back();
		return false;
	}
	//普通
	return false;

}
void Optimizer::PrintOptimizer(vector<Argnode>&t) {
	std::cout << t.size() << std::endl;
	for (int i = 0; i < t.size(); i++) {
		string op=t.at(i).codekind;
		string m1 = t.at(i).Arg1 == nullptr ? "-" : t.at(i).Arg1->name;
		//if (t.at(i).Arg1!=nullptr&&t.at(i).Arg1->form.compare("ValueForm")==0) m1 = std::to_string(t.at(i).Arg1->value);
		string m2=t.at(i).Arg2 == nullptr ? "-" : t.at(i).Arg2->name;
		string m3=t.at(i).res == nullptr ? "-" : t.at(i).res->name;
		
		cout << "( "
			<< "\t" << std::left << setw(8)
			<< op << " , "
			<< "\t" << std::left << setw(8)
			<< m1 << " , "
			<< "\t" << std::left << setw(8)
			<< m2 << " , "
			<< "\t" << std::left << setw(8)
			<< m3 << " ) " << endl;
	}
}
