#include "captchamovablelabel.h"


CaptchaMovableLabel::CaptchaMovableLabel(QWidget *parent) : QLabel(parent)
{
    effect.setOffset(0, 0);
    setGraphicsEffect(&effect);
}

void CaptchaMovableLabel::setAngle(int angle)
{
    this->prevAngle = this->angle;
    this->angle = angle;
}

void CaptchaMovableLabel::setColor(QColor color)
{
    this->prevColor = this->color;
    this->color = color;
}

void CaptchaMovableLabel::setText(QString text)
{
    this->prevCh = this->ch;
    this->ch = text;

    // 计算合适的高度
    QFontMetrics fm(this->font());
    double w = fm.horizontalAdvance(text)+2;
    double h = fm.height();

    const double PI = 3.141592;
    int xieHalf = sqrt(w*w/4+h*h/4); // 斜边的一半
    double a = atan(w/h) + CAPTCHA_CHAR_ANGLE_MAX * PI / 180; // 最大的倾斜角度
    int w2 = xieHalf * sin(a) * 2;

    a = atan(w/h) - CAPTCHA_CHAR_ANGLE_MAX * PI / 180;
    int h2 = xieHalf * cos(a) * 2;

    resize(w2, h2);
}

void CaptchaMovableLabel::startRefreshAnimation()
{
    if (!inited) // 第一次，直接显示，取消动画
    {
        inited = true;
        return ;
    }

    QPropertyAnimation* ani = new QPropertyAnimation(this, "refreshProgress");
    ani->setStartValue(0);
    ani->setEndValue(100);
    ani->setDuration(qrand() % (CAPTCHA_REFRESH_DURATION / 3) + CAPTCHA_REFRESH_DURATION / 3);
    ani->start();
    connect(ani, SIGNAL(finished()), ani, SLOT(deleteLater()));
}

QString CaptchaMovableLabel::text()
{
    return ch;
}

void CaptchaMovableLabel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setFont(this->font());
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
//    painter.drawRect(0, 0, width()-1, height()-1); // 绘制边框

    int w2 = width()/2, h2 = height()/2;
    painter.translate(w2, h2); // 平移到中心才进行旋转

    if (isNoAni())
    {
        painter.setPen(color);
        painter.rotate(angle);
        painter.drawText(QRect(-w2, -h2, width(), height()), Qt::AlignCenter, ch);
        return ;
    }

    // 动画里面，比较复杂
    double newProp = refreshProgress / 100.0;
    double oldProp = 1.0 - newProp;

//    double a = prevAngle * oldProp + angle * newProp + 0.5;
    painter.save();
    painter.rotate(prevAngle);

    QColor c = prevColor;
    c.setAlpha(c.alpha() * oldProp);
    painter.setPen(c);
    painter.drawText(QRect(-w2,-h2,width(),height()), Qt::AlignCenter, prevCh);
    painter.restore();

    painter.save();
    c = this->color;
    c.setAlpha(c.alpha() * newProp);
    painter.setPen(c);
    painter.rotate(angle);
    painter.drawText(QRect(-w2, -h2, width(), height()), Qt::AlignCenter, ch);
    painter.restore();
}

void CaptchaMovableLabel::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        // 开始拖拽
        press_pos = ev->pos();
        dragging = true;
        moved = false;
        effect.setOffset(-2, 2);
        effect.setBlurRadius(8);
        this->raise();
        return ev->accept();
    }
    QLabel::mousePressEvent(ev);
}

void CaptchaMovableLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if (dragging && ev->buttons() & Qt::LeftButton)
    {
        if (!moved && (ev->pos() - press_pos).manhattanLength() < QApplication::startDragDistance())
        {
            return QLabel::mouseMoveEvent(ev); // 还没到这时候
        }
        moved = true;
        move(this->pos() + ev->pos() - press_pos);
        ev->accept();
        return ;
    }
    QLabel::mouseMoveEvent(ev);
}

void CaptchaMovableLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (dragging)
    {
        // 结束拖拽
        dragging = false;
        effect.setOffset(0, 0);
        effect.setBlurRadius(0);
    }
    if (moved)
        return ev->accept();
    QLabel::mouseReleaseEvent(ev);
}

void CaptchaMovableLabel::startPressAnimation(int start, int end)
{

}

void CaptchaMovableLabel::setRefreshProgress(int g)
{
    this->refreshProgress = g;
    update();
}

int CaptchaMovableLabel::getRefreshProgress()
{
    return refreshProgress;
}

bool CaptchaMovableLabel::isNoAni()
{
    return refreshProgress == 100;
}
