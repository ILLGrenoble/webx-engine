#ifndef WEBX_WINDOW_COVERAGE_H
#define WEBX_WINDOW_COVERAGE_H

#include <vector>
#include <algorithm>
#include "WebXRectangle.h"

class WebXWindowCoverage {
private:
    // Event structure for the sweep line algorithm
    struct SweepLineEvent {
        int x, y1, y2, type; // x-coordinate, y range, event type (1 = entering, -1 = leaving)
        bool operator<(const SweepLineEvent& e) const {
            return x < e.x; // Sort by x-coordinate
        }
    };

public:
    WebXWindowCoverage() :
        coverage(0.0),
        mouseOver(false) {}
    WebXWindowCoverage(double coverage, bool mouseOver) :
        coverage(coverage),
        mouseOver(mouseOver) {}
    virtual ~WebXWindowCoverage() {}


    bool operator == (const WebXWindowCoverage & coverage) const {
        return this->coverage == coverage.coverage && this->mouseOver == coverage.mouseOver;
    }

    bool operator != (const WebXWindowCoverage & coverage) const {
        return !operator==(coverage);
    }

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

        // Check if mouse if over window
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