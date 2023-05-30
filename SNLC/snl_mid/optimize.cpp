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
		//���ڸ�ֵ��䣬�滻��һ��ARG�ṹ��
		this->searchBlock(this->table.at(this->point).Arg1);

		if (this->table.at(this->point).Arg1->form.compare("ValueForm") == 0) {
			//��������
			for (int i = 0; i < this->blocks.back().size(); i++) {
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

		//�Ƚ����滻
		this->searchBlock(this->table.at(this->point).Arg1);
		this->searchBlock(this->table.at(this->point).Arg2);

		if (this->table.at(this->point).Arg1->form.compare("ValueForm")==0 && this->table.at(this->point).Arg2->form.compare("ValueForm")==0) {
			this->addToBlock(this->table.at(this->point).res->name,
				this->table.at(this->point).Arg1->value + this->table.at(this->point).Arg2->value);
			vector<Argnode>::iterator it = this->table.begin();
			it += this->point;
			this->table.erase(it);
			this->point--;//��ѭ���̶���++��������ֹ����
		}
	}
	else if (this->table.at(this->point).codekind.compare("WRITE")==0) {
		//������ת��䡢�����䣬�滻��һ��ARG�ṹ
		this->searchBlock(this->table.at(this->point).Arg1);
	}
	else if (this->table.at(this->point).codekind.compare("AADD") == 0) {
		//��ַ������
			
	}
}

bool Optimizer::addToBlock(string name, int val) {

	Twotuple* tuple = new Twotuple();
	tuple->name = name;//������ֵ��������
	tuple->value = val;//д�볣��ֵ
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

		//����һ���µĿ�
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
		//����һ����
		this->point++;
		this->blocks.pop_back();
		return false;
	}
	//��ͨ
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
