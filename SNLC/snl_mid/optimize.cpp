#include"optimize.h"

void Optimizer::ConstOptimize() {
	
	
	while (this->point < table.size()) {
		this->DivBaseBlock();
		this->OptiBlock();
	}

}

void Optimizer::OptiBlock() {

	if (this->table.at(this->point).codekind.compare(std::to_string("ASSIGN")) == 0) {
		//���ڸ�ֵ��䣬�滻��һ��ARG�ṹ��
		this->searchBlock(this->table.at(this->point).Arg1);

		if (this->table.at(this->point).Arg1->form.compare(std::to_string("ValueForm")) == 0) {
			//��������
			for (int i = 0; i < this->blocks.back().size; i++) {
				if (this->blocks.back().at(i)->name == this->table.at(this->point).Arg2->name) {
					this->blocks.back().at(i)->value = this->table.at(this->point).Arg1->value;
					return;
				}
			}

			Twotuple* tuple = new Twotuple();
			tuple->name = this->table.at(this->point).Arg2->name;//������ֵ��������
			tuple->value = this->table.at(this->point).Arg1->value;//д�볣��ֵ
			this->blocks.back().push_back(tuple);
		}
		else {
			//�ǳ�����
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

		//�Ƚ����滻
		this->searchBlock(this->table.at(this->point).Arg1);
		this->searchBlock(this->table.at(this->point).Arg2);

		if (this->table.at(this->point).Arg1->form.compare(std::to_string("ValueForm")) && this->table.at(this->point).Arg2->form.compare(std::to_string("ValueForm"))) {
			this->addToBlock(this->table.at(this->point).res->name,
				this->table.at(this->point).Arg1->value + this->table.at(this->point).Arg2->value);
			this->table.erase(this->point);
			this->point--;//��ѭ���̶���++��������ֹ����
		}
	}
	else if (this->table.at(this->point).codekind.compare(std::to_string("WRITE")) == 00) {
		//������ת��䡢�����䣬�滻��һ��ARG�ṹ
		this->searchBlock(this->table.at(this->point).Arg1);
	}
	else if (this->table.at(this->point).codekind.compare(std::to_string("AADD")) == 00) {
		//��ַ������
			
	}
}

bool Optimizer::addToBlock(string name, int val) {

	Twotuple* tuple = new Twotuple();
	tuple->name = name;//������ֵ��������
	tuple->value = val;//д�볣��ֵ
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

		//����һ���µĿ�
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
		//����һ����
		this->point++;
		this->blocks.pop_back();
		return false;
	}
	//��ͨ
	return false;

}