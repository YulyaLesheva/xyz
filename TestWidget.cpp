#include "stdafx.h"
#include "TestWidget.h"

TestWidget::TestWidget(const std::string& name, rapidxml::xml_node<>* elem)
	: Widget(name)
	, _curTex(0)
	, _timer(0)
	, _newTimer(0)
	, _angle(0)
	, _eff(NULL)
	, _scale(0.f)
	,_scaleBomb()
	, _startPos(0, 0, 0)
	, _velocityVec(1, 0, 0)
	, _vectorLegth(0)
	, _finalPos(500, 500, 0)
	, circlePoint(1, 0)
	, _bombPoint(500, 0)
	
{
	Init();
}

void TestWidget::Init()
{
	_tex1 = Core::resourceManager.Get<Render::Texture>("btnStart_Text");
	_tex2 = Core::resourceManager.Get<Render::Texture>("Circle"); 
	_tex3 = Core::resourceManager.Get<Render::Texture>("Star");
	_tex4 = Core::resourceManager.Get<Render::Texture>("Target");
	_tex5 = Core::resourceManager.Get<Render::Texture>("Bomb");
	_tex6 = Core::resourceManager.Get<Render::Texture>("Target");

	_curTex = 0;
	_angle = 0;

	spline.addKey(0.0f, FPoint(100.0f, 100.0f));
	spline.addKey(0.25f, FPoint(150.0f, 300.0f));
	spline.addKey(0.5f, FPoint(500.0f, 300.0f));
	spline.addKey(0.75f, FPoint(630.0f, 450.0f));
	spline.addKey(1.0f, FPoint(600.0f, 550.0f));
	spline.CalculateGradient();
}

void TestWidget::Draw()
{
	//
	// Получаем текущее положение курсора мыши.
	//
	IPoint mouse_pos = Core::mainInput.GetMousePos();

	//
	// Проталкиваем в стек текущее преобразование координат, чтобы в дальнейшем
	// можно было восстановить это преобразование вызовом PopMatrix.
	//
	Render::device.PushMatrix();
	
	//
	// Изменяем текущее преобразование координат, перемещая центр координат в позицию мыши
	// и поворачивая координаты относительно этого центра вокруг оси z на угол _angle.
	//
	Render::device.MatrixTranslate((float)mouse_pos.x, (float)mouse_pos.y, 0);
	Render::device.MatrixRotate(math::Vector3(0, 0, 1), _angle);
	///Render::device.MatrixScale(5);


	if (!_curTex)
	{
		//
		// Метод Texture::Draw() выводит квадратный спрайт с размерами текстуры
		// в центре координат (0, 0). Центр координат и преобразование координат вершин
		// спрайта устанавливаются с применением текущего преобразования.
		//
		// При вызове метода Texture::Draw() вызывать Texture::Bind() необязательно.
		//
		_tex1->Draw();
	}
	else
	{
		IRect texRect = _tex2->getBitmapRect();

		//
		// При отрисовке текстуры можно вручную задавать UV координаты той части текстуры,
		// которая будет натянута на вершины спрайта. UV координаты должны быть нормализованы,
		// т.е., вне зависимости от размера текстуры в текселях, размер любой текстуры
		// равен 1 (UV координаты задаются в диапазоне 0..1, хотя ничто не мешает задать их
		// больше единицы, при этом в случае установленной адресации текстуры REPEAT, текстура
		// будет размножена по этой стороне соответствующее количество раз).
		//

		FRect rect(texRect);
		FRect uv(0, 1, 0, 1);

		_tex2->TranslateUV(rect, uv);

		Render::device.MatrixScale(_scale);
		Render::device.MatrixTranslate(-texRect.width * 0.5f, -texRect.height * 0.5f, 0.0f);

		//
		// Привязываем текстуру.
		//
		_tex2->Bind();
		
		//
		// Метод DrawQuad() выводит в графическое устройство квадратный спрайт, состоящий их двух
		// примитивов - треугольников, используя при этом текущий цвет для вершин и привязанную (binded) текстуру,
		// если разрешено текстурирование.
		//
		// Перед вызовом DrawQuad() должен быть вызов Texture::Bind() либо SetTexturing(false),
		// иначе визуальный результат будет непредсказуемым.
		//
		Render::DrawQuad(rect, uv);
	}

	//
	// Воостанавливаем прежнее преобразование координат, снимая со стека изменённый фрейм.
	//
	Render::device.PopMatrix();
	
	//
	// Получаем текущие координаты объекта, двигающегося по сплайну
	//
	FPoint currentPosition = spline.getGlobalFrame(math::clamp(0.0f, 1.0f, _timer / 6.0f));

	//
	// И рисуем объект в этих координатах
	//
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(currentPosition.x, currentPosition.y, 0);
	_tex3->Draw();
	Render::device.PopMatrix();

	//
	// Этот вызов отключает текстурирование при отрисовке.
	//
	Render::device.SetTexturing(false);
	
	//
	// Метод BeginColor() проталкивает в стек текущий цвет вершин и устанавливает новый.
	//
	Render::BeginColor(Color(255, 128, 0, 255));
	
	//
	// Метод DrawRect() выводит в графическое устройство квадратный спрайт, состоящий их двух
	// примитивов - треугольников, используя при этом текущий цвет для вершин и привязанную (binded) текстуру,
	// если разрешено текстурирование.
	//
	// Перед вызовом DrawRect() должен быть вызов Texture::Bind(), либо SetTexturing(false),
	// иначе визуальный результат будет непредсказуемым.
	//
	Render::DrawRect(924, 0, 100, 100);
	
	//
	// Метод EndColor() снимает со стека текущий цвет вершин, восстанавливая прежний.
	//
	Render::EndColor();
	
	//
	// Опять включаем текстурирование.
	//
	Render::device.SetTexturing(true);

	//
	// Рисуем все эффекты, которые добавили в контейнер (Update() для контейнера вызывать не нужно).
	//
	_effCont.Draw();

	Render::BindFont("arial");
	Render::PrintString(924 + 100 / 2, 35, utils::lexical_cast(mouse_pos.x) + ", " + utils::lexical_cast(mouse_pos.y), 1.f, CenterAlign);
	
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(_startPos);
	_tex4->Draw(IPoint(0,0));
	circlePoint = getPosition();
	Render::device.PopMatrix();
	
	Render::device.PushMatrix();
	Render::device.MatrixTranslate(_bombPoint);
	_tex5->Draw();
	Render::device.PopMatrix();

	auto width = _tex6->Width();
	auto height = _tex6->Height();

	Render::device.PushMatrix();
	Render::device.MatrixTranslate(-width *0.5, - height * 0.5, 0.0f);
	Render::device.MatrixTranslate(1024 * 0.5, 768 * 0.5, 0);
	_tex6->Draw();
	Render::device.PopMatrix();

	
}

void TestWidget::Update(float dt)
{
	//
	// Обновим контейнер с эффектами
	//
	_effCont.Update(dt);

	//
	// dt - значение времени в секундах, прошедшее от предыдущего кадра.
	// Оно может принимать разные значения, в зависимости от производительности системы
	// и сложности сцены.
	//
	// Для того, чтобы наша анимация зависела только от времени, и не зависела от
	// производительности системы, мы должны рассчитывать её от этого значения.
	//
	// Увеличиваем наш таймер с удвоенной скоростью.
	//
	_timer += dt * 1;
	_newTimer += dt * 1;
	//
	// Зацикливаем таймер в диапазоне (0, 2п).
	// Это нужно делать для предотвращения получения некорректных значений,
	// если вдруг переполнится разрядная сетка (float переполнился) или задержка
	// от предыдущего кадра была слишкой большой (система тормози-и-ит).
	//
	// Диапазон значений выбран равным (0, 2п), потому что мы используем это значение
	// для расчёта синуса, и большие значения будут просто периодически повторять результат.
	//
	while (_timer > 2 * math::PI)
	{
		_timer -= 2 * math::PI;
	}
	
	//
	// Анимирование параметра масштабирования в зависимости от таймера.
	//
	_scale = 0.8f + 0.25f * sinf(_timer);
	
	_startPos += _velocityVec * _newTimer; /// можно убрать множитель на время

	if (_startPos.x > 1000) {
		_startPos.x = 0;
	}

	math::Vector3 twoPoints = math::Vector3(_startPos.x - _bombPoint.x, _startPos.y - _bombPoint.y, 0);
	_vectorLegth = twoPoints.Length();

	if (_vectorLegth <= 100) {
		_bombPoint = IPoint(0, 0);
	}

}

bool TestWidget::MouseDown(const IPoint &mouse_pos)
{
	if (Core::mainInput.GetMouseRightButton())
	{
		//
		// При нажатии на правую кнопку мыши, создаём эффект шлейфа за мышкой.
		//
		_eff = _effCont.AddEffect("Iskra");
		_eff->posX = mouse_pos.x + 0.f;
		_eff->posY = mouse_pos.y + 0.f;
		_eff->Reset();
		
		//
		// И изменяем угол наклона текстуры.
		//
		_angle += 25;
		while (_angle > 360)
		{
			_angle -= 360;
		}
	}
	else
	{
		//
		// При нажатии на левую кнопку мыши, создаём временный эффект, который завершится сам.
		//
		ParticleEffectPtr eff = _effCont.AddEffect("FindItem2");
		eff->posX = mouse_pos.x + 0.f;
		eff->posY = mouse_pos.y + 0.f;
		eff->Reset();

		//
		// Изменяем значение с 0 на 1 и наоборот.
		//
		_curTex = 1 - _curTex;
	}
	return false;
}

void TestWidget::MouseMove(const IPoint &mouse_pos)
{
	if (_eff)
	{
		//
		// Если эффект создан, то перемещаем его в позицию мыши.
		//
		_eff->posX = mouse_pos.x + 0.f;
		_eff->posY = mouse_pos.y + 0.f;
	}
}

void TestWidget::MouseUp(const IPoint &mouse_pos)
{
	if (_eff)
	{
		//
		// Если эффект создан, то при отпускании мыши завершаем его.
		//
		_eff->Finish();
		_eff = NULL;
	}
}

void TestWidget::AcceptMessage(const Message& message)
{
	//
	// Виджету могут посылаться сообщения с параметрами.
	//

	const std::string& publisher = message.getPublisher();
	const std::string& data = message.getData();
}

void TestWidget::KeyPressed(int keyCode)
{
	//
	// keyCode - виртуальный код клавиши.
	// В качестве значений для проверки нужно использовать константы VK_.
	//

	if (keyCode == VK_A) {
		// Реакция на нажатие кнопки A
	}
}

void TestWidget::CharPressed(int unicodeChar)
{
	//
	// unicodeChar - Unicode код введённого символа
	//

	if (unicodeChar == L'а') {
		// Реакция на ввод символа 'а'
	}
}
