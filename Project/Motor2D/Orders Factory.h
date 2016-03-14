#ifndef _ORDERS_FACTORY_H_
#define _ORDERS_FACTORY_H_

#define NULL 0

class UIButton;


class Order
{
public:
	Order(){ button = NULL; }

	virtual void Function(){}

	//Setters & Getters
	void SetButton(UIButton& nButt){ button = &nButt; }

	const UIButton* getButton() const { return button; }

private:
	UIButton* button;


};

struct Gen_probe : public Order
{
	Gen_probe() :Order(){}
	void Function();
	
};

struct Attack : public Order
{
	Attack() :Order(){}
	void Function();
	
};
#endif // !_ORDERS_FACTORY_H_
