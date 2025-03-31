#ifndef WEBX_WINDOW_COVERAGE_H
#define WEBX_WINDOW_COVERAGE_H

#include <vector>
#include <algorithm>
#include "WebXRectangle.h"

/**
 * @class WebXWindowCoverage
 * @brief Represents the coverage of a window, including the percentage of the window covered and whether the mouse is over it.
 */
class WebXWindowCoverage {
private:
    /**
     * @struct SweepLineEvent
     * @brief Represents an event in the sweep line algorithm for calculating coverage.
     */
    struct SweepLineEvent {
        int x, y1, y2, type; /**< x-coordinate, y range, and event type (1 = entering, -1 = leaving). */
        bool operator<(const SweepLineEvent& e) const {
            return x < e.x; /**< Sort by x-coordinate. */
        }
    };

public:
    /**
     * @brief Default constructor for WebXWindowCoverage.
     * Initializes coverage to 0.0 and mouseOver to false.
     */
    WebXWindowCoverage() :
        coverage(0.0),
        mouseOver(false) {}

    /**
     * @brief Constructs a WebXWindowCoverage object with specified coverage and mouseOver state.
     * @param coverage The percentage of the window covered (0.0 to 1.0).
     * @param mouseOver Whether the mouse is over the window.
     */
    WebXWindowCoverage(double coverage, bool mouseOver) :
        coverage(coverage),
        mouseOver(mouseOver) {}

    /**
     * @brief Destructor for WebXWindowCoverage.
     */
    virtual ~WebXWindowCoverage() {}

    /**
     * @brief Equality operator for WebXWindowCoverage.
     * @param coverage The WebXWindowCoverage object to compare with.
     * @return True if both coverage and mouseOver are equal, false otherwise.
     */
    bool operator==(const WebXWindowCoverage & coverage) const {
        return this->coverage == coverage.coverage && this->mouseOver == coverage.mouseOver;
    }

    /**
     * @brief Inequality operator for WebXWindowCoverage.
     * @param coverage The WebXWindowCoverage object to compare with.
     * @return True if either coverage or mouseOver are not equal, false otherwise.
     */
    bool operator!=(const WebXWindowCoverage & coverage) const {
        return !operator==(coverage);
    }

    /**
     * @brief Calculates the overlap coverage of a base rectangle with a set of covering rectangles.
     * @param base The base rectangle.
     * @param coveringRectangles A vector of rectangles covering the base rectangle.
     * @param mouseX The x-coordinate of the mouse position.
     * @param mouseY The y-coordinate of the mouse position.
     * @return A WebXWindowCoverage object representing the coverage percentage and mouseOver state.
     */
    static WebXWindowCoverage OverlapCalc(const WebXRectangle & base, const std::vector<WebXRectangle> & coveringRectangles, int mouseX, int mouseY) {
        std::vector<SweepLineEvent> events;

        // Collect all vertical edges as events
        for (const auto& rect : coveringRectangles) {
            if (!base.overlap(rect)) {
                // Ignore completely outside rectangles
                continue;
            }
    
            events.push_back({std::max(rect.left(), base.left()), std::max(rect.bottom(), base.bottom()), std::min(rect.top(), base.top()), 1});
            events.push_back({std::min(rect.right(), base.right()), std::max(rect.bottom(), base.bottom()), std::min(rect.top(), base.top()), -1});
        }
    
        // Sort events by x-coordinate
        std::sort(events.begin(), events.end());
    
        int prevX = base.left();
        int coveredArea = 0;
        std::multiset<std::pair<int, int>> activeIntervals; // Stores active y-intervals

        // Check if mouse is over the window
        bool mouseOver = (base.right() >= mouseX && base.left() <= mouseX && base.top() >= mouseY && base.bottom() <= mouseY);

        for (const auto& event : events) {
            int currentX = event.x;
    
            // Calculate the covered vertical range
            int coveredHeight = 0;
            int lastY = 0;
    
            for (const auto& interval : activeIntervals) {
                int y1 = interval.first;
                int y2 = interval.second;

                int lowerY = lastY < y1 ? y1 : lastY;
                coveredHeight += (y2 - lowerY);

                // Check if coverage includes the mouse position
                if (currentX >= mouseX && prevX <= mouseX && y2 >= mouseY && lowerY <= mouseY) {
                    mouseOver = false;
                }
    
                lastY = std::max(lastY, y2);
            }
    
            // Add the covered area from the last segment
            coveredArea += coveredHeight * (currentX - prevX);
            prevX = currentX;
    
            // Add or remove intervals based on event type
            if (event.type == 1) {
                activeIntervals.insert({event.y1, event.y2});
            } else {
                activeIntervals.erase(activeIntervals.find({event.y1, event.y2}));
            }
        }

        return WebXWindowCoverage((double)coveredArea / base.area(), mouseOver);
    }

public: 
    double coverage;
    bool mouseOver;
};

#endif /* WEBX_WINDOW_COVERAGE_H */