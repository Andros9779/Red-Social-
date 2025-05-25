#include "TimelineWidget.h"
#include <QDateTime>
#include <algorithm>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QLabel>

TimelineWidget::TimelineWidget(Graph& graph, uint64_t userId, QWidget* parent)
    : QWidget(parent), g(graph), userId(userId) {
    mainLayout = new QVBoxLayout(this);
    postEdit = new QTextEdit(this);
    postButton = new QPushButton("Publicar", this);
    postList = new QListWidget(this);

    // Assemble layout
    mainLayout->addWidget(postEdit);
    mainLayout->addWidget(postButton);
    mainLayout->addWidget(postList);

    // Connect button
    connect(postButton, &QPushButton::clicked, this, &TimelineWidget::onPost);

    // Initial load
    refresh();
}

void TimelineWidget::onPost() {
    QString content = postEdit->toPlainText().trimmed();
    if (content.isEmpty())
        return;
    g.addPost(userId, content.toStdString());
    postEdit->clear();
    refresh();
}

void TimelineWidget::refresh() {
    postList->clear();
    auto userPosts = g.getFeed(userId);

    // Sort posts by most recent first
    std::sort(userPosts.begin(), userPosts.end(),
              [](const Post& a, const Post& b) {
                  return a.timestamp > b.timestamp;
              });

    for (const auto& p : userPosts) {
        QDateTime dt = QDateTime::fromSecsSinceEpoch(p.timestamp);
        // Build display text with author
        User* author = g.getUser(p.userId);
        QString authorName = author ? QString::fromStdString(author->name) : QString("Unknown");
        QString content = dt.toString("yyyy-MM-dd hh:mm:ss") + " [" + authorName + "] " + 
                          QString::fromStdString(p.text);

        // Create list item and container widget
        QListWidgetItem* item = new QListWidgetItem(postList);
        QWidget* container = new QWidget(postList);
        QHBoxLayout* hLayout = new QHBoxLayout(container);

        // Label for content
        QLabel* label = new QLabel(content, container);
        hLayout->addWidget(label);

        // Like button
        QPushButton* likeBtn = new QPushButton(QString("👍 %1").arg(p.likes), container);
        connect(likeBtn, &QPushButton::clicked, this, [this, p]() {
            g.likePost(p.userId, p.timestamp);
            refresh();
        });
        hLayout->addWidget(likeBtn);

        // Comment button
        QPushButton* commentBtn = new QPushButton("💬", container);
        connect(commentBtn, &QPushButton::clicked, this, [this, p]() {
            bool ok;
            QString cm = QInputDialog::getText(this, "Comentario", "Tu comentario:", QLineEdit::Normal, "", &ok);
            if (ok && !cm.trimmed().isEmpty()) {
                g.addComment(p.userId, p.timestamp, cm.toStdString());
                refresh();
            }
        });
        hLayout->addWidget(commentBtn);

        // Set layout margins to zero
        hLayout->setContentsMargins(0,0,0,0);

        // Assign widget to item
        container->setLayout(hLayout);
        postList->setItemWidget(item, container);
        item->setSizeHint(container->sizeHint());
    }
}