#pragma once

///
/// Виджет - основной визуальный элемент на экране.
/// Он отрисовывает себя, а также может содержать другие виджеты.
///
class TestWidget : public GUI::Widget
{
public:
	TestWidget(const std::string& name, rapidxml::xml_node<>* elem);

	void Draw() override;
	void Update(float dt) override;
	
	void AcceptMessage(const Message& message) override;
	
	bool MouseDown(const IPoint& mouse_pos) override;
	void MouseMove(const IPoint& mouse_pos) override;
	void MouseUp(const IPoint& mouse_pos) override;

	void KeyPressed(int keyCode) override;
	void CharPressed(int unicodeChar) override;

private:
	void Init();

private:
	float _timer;
	float _newTimer;
	float _scale;
	float _scaleBomb;
	float _angle;
	float _vectorLegth;
	IPoint circlePoint;
	IPoint _bombPoint;
	Render::Texture* _tex1;
	Render::Texture* _tex2;
	Render::Texture* _tex3;
	Render::Texture* _tex4;
	Render::Texture* _tex5;
	Render::Texture* _tex6;


	math::Vector3 _startPos;
	math::Vector3 _velocityVec;
	math::Vector3 _finalPos;

	
	int _curTex;

	EffectsContainer _effCont;
	ParticleEffectPtr _eff;

	TimedSpline<FPoint> spline;
};
