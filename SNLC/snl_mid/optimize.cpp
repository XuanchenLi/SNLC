#include"optimize.h"

void Optimizer::ConstOptimize() {
	
	
	while (this->point < table.size()) {
		this->DivBaseBlock();
		this->OptiBlock();
	}

}

void Optimizer::OptiBlock() {

	if (this->table.at(this->point).codekind.compare(std::to_string("ASSIGN")) == 0) {
		//对于赋值语句，替换第一个ARG结构；
		this->searchBlock(this->table.at(this->point).Arg1);

		if (this->table.at(this->point).Arg1->form.compare(std::to_string("ValueForm")) == 0) {
			//常数加入
			for (int i = 0; i < this->blocks.back().size; i++) {
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
			for (int i = 0; i < this->blocks.back().size; i++) {
				if (this->blocks.back().at(i)->name == this->table.at(this->point).Arg2->name) {
					this->blocks.back().erase(i);
					return;
				}
			}
		}
	}
	else if (this->table.at(this->point).codekind.compare(std::to_string("+")) == 0 ||
		this->table.at(this->point).codekind.compare(std::to_string("-")) == 0 ||
		this->table.at(this->point).codekind.compare(std::to_string("*")) == 0 ||
		this->table.at(this->point).codekind.compare(std::to_string("/")) == 0 ||
		this->table.at(this->point).codekind.compare(std::to_string("<")) == 0 ||
		this->table.at(this->point).codekind.compare(std::to_string(">")) == 0) {

		//先进行替换
		this->searchBlock(this->table.at(this->point).Arg1);
		this->searchBlock(this->table.at(this->point).Arg2);

		if (this->table.at(this->point).Arg1->form.compare(std::to_string("ValueForm")) && this->table.at(this->point).Arg2->form.compare(std::to_string("ValueForm"))) {
			this->addToBlock(this->table.at(this->point).res->name,
				this->table.at(this->point).Arg1->value + this->table.at(this->point).Arg2->value);
			this->table.erase(this->point);
			this->point--;//和循环固定的++抵消，防止跳过
		}
	}
	else if (this->table.at(this->point).codekind.compare(std::to_string("WRITE")) == 00) {
		//条件跳转语句、输出语句，替换第一个ARG结构
		this->searchBlock(this->table.at(this->point).Arg1);
	}
	else if (this->table.at(this->point).codekind.compare(std::to_string("AADD")) == 00) {
		//地址加运算
			
	}
}

bool Optimizer::addToBlock(string name, int val) {

	Twotuple* tuple = new Twotuple();
	tuple->name = name;//将被赋值变量加入
	tuple->value = val;//写入常数值
	this->blocks.back().push_back(tuple);
}

bool Optimizer::searchBlock(ArgRecord& arg) {

	for (int i = 0; i < this->blocks.back().size; i++) {
		if (this->blocks.back().at(i)->name == arg.name) {
			arg.value=this->blocks.back().at(i)->value;
			arg.form = "ValueForm";
			return true;
		}
	}
	return false;
}

bool Optimizer::DivBaseBlock(){

	if (this->table.at(this->point).codekind.compare(std::to_string("ENTRY")) == 0|| 
		this->table.at(this->point).codekind.compare(std::to_string("WHILE")) == 0|| 
		this->table.at(this->point).codekind.compare(std::to_string("THEN")) == 0||
		this->table.at(this->point).codekind.compare(std::to_string("ELSE")) == 0) {

		//进入一个新的块
		this->point++;
		std::vector<Twotuple*> block;
		this->blocks.push_back(block);
		return true;
	}
	else if (this->table.at(this->point).codekind.compare(std::to_string("ENDPROC")) == 0 || 
		this->table.at(this->point).codekind.compare(std::to_string("ENDWHILE")) == 0 ||
		this->table.at(this->point).codekind.compare(std::to_string("ENDIF")) == 0 ||
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