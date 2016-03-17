#ifndef _ORDERS_FACTORY_H_
#define _ORDERS_FACTORY_H_

class UI_Button2;
class UI_Rect;
class UI_Image;

class Order
{
public:
	Order(){ button = nullptr; }

	virtual void Function(){}

	//Setters & Getters
	void SetButton(UI_Button2& nButt){ button = &nButt; }

	const UI_Button2* getButton() const { return button; }

private:
	UI_Button2* button;


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
