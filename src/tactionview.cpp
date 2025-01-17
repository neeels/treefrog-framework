/* Copyright (c) 2010-2015, AOYAMA Kazuharu
 * All rights reserved.
 *
 * This software may be used and distributed according to the terms of
 * the New BSD License, which is incorporated herein by reference.
 */

#include <TActionView>
#include <TWebApplication>
#include <TActionController>
#include <THttpUtility>
#include <THtmlAttribute>
#include <TReactComponent>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>
#include "tsystemglobal.h"

/*!
  \class TActionView
  \brief The TActionView class is the abstract base class of views,
  providing functionality common to view.
*/


/*!
  Constructor.
*/
TActionView::TActionView()
    : QObject(), TViewHelper(), TPrototypeAjaxHelper(), actionController(0), subView(0)
{ }

/*!
  Returns a content processed by a action.
*/
QString TActionView::yield() const
{
    return (subView) ? subView->toString() : QString();
}

/*!
  Render the partial template given by \a templateName without layout.
*/
QString TActionView::renderPartial(const QString &templateName, const QVariantMap &vars) const
{
    QString temp = templateName;
    if (!temp.contains('/')) {
        temp = QLatin1String("partial") + QDir::separator() + temp;
    }
    return (actionController) ? actionController->getRenderingData(temp, vars) : QString();
}

/*!
  Renders the React \a component on the server. Calls ReactDOMServer.renderToString()
  internally.
*/
QString TActionView::renderReact(const QString &component)
{
    static QMap<QString, TReactComponent*> reactComponents;
    static QMutex mutex;

    if (component.isEmpty()) {
        return QString();
    }

    QMutexLocker locker(&mutex);
    TReactComponent *react = reactComponents.value(component);

    if (react) {
        QDateTime modified = QFileInfo(react->filePath()).lastModified();
        if (!modified.isValid() || modified > react->loadedDateTime()) {
            // Removes the item to reload
            reactComponents.remove(component);
            delete react;
            react = nullptr;
        }
    }
    locker.unlock();

    if (!react) {
        QDir dir(Tf::app()->publicPath() + "js" + QDir::separator() + "components");
        QStringList filter = { component + ".*" };
        QString file = dir.entryList(filter, QDir::Files).value(0);

        if (!file.isEmpty()) {
            react = new TReactComponent(dir.absolutePath() + QDir::separator() + file);
            locker.relock();
            reactComponents.insert(component, react);
            locker.unlock();
        } else {
            return QString();
        }
    }
    return react->renderToString(component);
}

/*!
  Returns a authenticity token for CSRF protection.
*/
QString TActionView::authenticityToken() const
{
    return (actionController) ? QString::fromLatin1(actionController->authenticityToken().data()) : QString();
}

/*!
  Outputs the string of the HTML attribute \a attr to a view
  template.
*/
QString TActionView::echo(const THtmlAttribute &attr)
{
    responsebody += attr.toString().trimmed();
    return QString();
}

/*!
  Outputs a escaped string of the HTML attribute \a attr to
  a view template.
*/
QString TActionView::eh(const THtmlAttribute &attr)
{
    return echo(THttpUtility::htmlEscape(attr.toString().trimmed()));
}

/*!
  Returns the requested HTTP message.
*/
const THttpRequest &TActionView::httpRequest() const
{
    return controller()->httpRequest();
}


/*!
  \fn QString TActionView::echo	(const QString &str)
  Outputs the string \a str to a view template.
*/

/*!
  \fn QString TActionView::echo(const char *str)
  Outputs the string \a str to a view template.
*/

/*!
  \fn QString TActionView::echo(const QByteArray &str)
  Outputs the array \a str to a view template.
*/

/*!
  \fn QString TActionView::echo(int n, int base)
  Outputs the number \a n to a view template.
*/

/*!
  \fn QString TActionView::echo(double d, char format, int precision)
  Outputs the number \a d to a view template.
*/

/*!
  \fn QString TActionView::echo(const QVariant &var)
  Outputs the variant variable \a var to a view template.
*/

/*!
  \fn QString TActionView::eh(const QString &str)
  Outputs a escaped string of the \a str to a view template.
*/

/*!
  \fn QString TActionView::eh(const char *str)
  Outputs a escaped string of the \a str to a view template.
*/

/*!
  \fn QString TActionView::eh(const QByteArray &str)
  Outputs a escaped array of the \a str to a view template.
*/

/*!
  \fn QString TActionView::eh(int n, int base)
  Outputs the number \a n to a view template.
*/

/*!
  \fn QString TActionView::eh(double d, char format, int precision)
  Outputs the number \a d to a view template.
*/

/*!
  \fn QString TActionView::eh(const QVariant &var)
  Outputs a escaped string of the variant variable \a var
  to a view template.
*/

/*!
  \fn bool TActionView::hasVariant(const QString &name) const
  Returns true if the QVariantMap variable for a view contains
  an item with the \a name; otherwise returns false.
*/

/*!
  \fn virtual QString TActionView::toString()
  This function is reimplemented in subclasses to return a
  string to render a view.
*/

/*!
  \fn QVariant TActionView::variant(const QString &name) const
  Returns the value associated with the \a name in the QVariantMap
  variable for a view.
*/
