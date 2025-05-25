

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include "graph.h"

/**
 * @class TimelineWidget
 * @brief Widget to display and create user posts (status updates).
 */
class TimelineWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs the timeline for a given user.
     * @param graph Reference to the Graph backend.
     * @param userId ID of the current user.
     * @param parent Parent widget pointer.
     */
    explicit TimelineWidget(Graph& graph, uint64_t userId, QWidget* parent = nullptr);

public slots:
    /**
     * @brief Refreshes the displayed list of posts.
     */
    void refresh();

private slots:
    /**
     * @brief Handler for when the user clicks "Publicar".
     */
    void onPost();

private:
    Graph& g;                  ///< Reference to the social graph.
    uint64_t userId;           ///< ID of the current user.
    QVBoxLayout* mainLayout;   ///< Main vertical layout.
    QTextEdit* postEdit;       ///< Text input for new post.
    QPushButton* postButton;   ///< Button to publish new post.
    QListWidget* postList;     ///< List widget showing posts.
};