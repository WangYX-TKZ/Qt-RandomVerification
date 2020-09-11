#ifndef CAPTCHALABEL_H
#define CAPTCHALABEL_H

#include "captchamovablelabel.h"
#include <QTimer>

#define CAPTCHAR_COUNT 4 // 验证码字符数量

class CaptchaLabel : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int refreshProgress READ getRefreshProgress WRITE setRefreshProgress)
public:
    CaptchaLabel(QWidget* parent = nullptr);

    void refresh();
    bool match(QString input);

private:
    void initView();
    void setRefreshProgress(int g);
    int getRefreshProgress();
    bool isNoAni();

protected:
    void paintEvent(QPaintEvent* ) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    CaptchaMovableLabel* charLabels[CAPTCHAR_COUNT]; // Label控件
    QList<QPoint> noisePoints; // 噪音点
    QList<QColor> pointColors; // 点的颜色
    QList<QLine> noiseLines; // 噪音线
    QList<QGradient> lineColors; // 线的渐变色

    int refreshProgress = 100;
    QList<QPoint> noisePoints2; // 新的位置
};

#endif // CAPTCHALABEL_H
