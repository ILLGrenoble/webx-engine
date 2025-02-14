#ifndef WEBX_RECTANGLE_H
#define WEBX_RECTANGLE_H

#include <vector>
#include <algorithm>
#include <set>
#include "WebXSize.h"

class WebXRectangle {

private:
    // Event structure for the sweep line algorithm
    struct SweepLineEvent {
        int x, y1, y2, type; // x-coordinate, y range, event type (1 = entering, -1 = leaving)
        bool operator<(const SweepLineEvent& e) const {
            return x < e.x; // Sort by x-coordinate
        }
    };

public:
    WebXRectangle() :
        x(0),
        y(0),
        _left(0),
        _right(0),
        _top(0),
        _bottom(0) {}
    WebXRectangle(int x, int y, int width, int height) :
        x(x),
        y(y),
        size(WebXSize(width, height)),
        _left(x),
        _right(x + width),
        _top(y + height),
        _bottom(y) {}
    WebXRectangle(const WebXRectangle & rectangle) :
        x(rectangle.x),
        y(rectangle.y),
        size(rectangle.size),
        _left(rectangle._left),
        _right(rectangle._right),
        _top(rectangle._top),
        _bottom(rectangle._bottom) {}
    virtual ~WebXRectangle() {}

    WebXRectangle & operator=(const WebXRectangle & rectangle) {
        if (this != &rectangle) {
            this->x = rectangle.x;
            this->y = rectangle.y;
            this->size = rectangle.size;
            this->_left =rectangle._left;
            this->_right =rectangle._right;
            this->_top =rectangle._top;
            this->_bottom =rectangle._bottom;
        }
        return *this;
    }

    bool isVisible(const WebXSize & viewport) const {
        return
            this->_left < viewport.width &&
            this->_right > 0 && 
            this->_bottom < viewport.height &&
            this->_top > 0;
    }

    bool overlapOrTouching(const WebXRectangle & rectangle) const {
        return (
            this->_left < (rectangle._right + 1) &&
            this->_right > (rectangle._left - 1) &&
            this->_top > (rectangle._bottom - 1) &&
            this->_bottom < (rectangle._top + 1));
    }

    bool overlap(const WebXRectangle & rectangle) const {
        return (
            this->_left < rectangle._right &&
            this->_right > rectangle._left &&
            this->_top > rectangle._bottom &&
            this->_bottom < rectangle._top);
    }

    float overlapCoeff(const WebXRectangle & rectangle) const {
        int maxLeft = this->_left > rectangle._left ? this->_left : rectangle._left;
        int minRight = this->_right < rectangle._right ? this->_right : rectangle._right;
        int minTop = this->_top < rectangle._top ? this->_top : rectangle._top;
        int maxBottom = this->_bottom > rectangle._bottom ? this->_bottom : rectangle._bottom;

        int overlapWidth = minRight - maxLeft;
        int overlapHeight = minTop - maxBottom;

        if (overlapWidth <= 0 || overlapHeight <= 0) {
            return 0.0;
        }

        int intersectionArea = overlapWidth * overlapHeight;

        return (double)intersectionArea / this->area();
    }

    float overlapCoeff(const std::vector<WebXRectangle> & coveringRectangles) const {
        std::vector<SweepLineEvent> events;

        // Collect all vertical edges as events
        for (const auto& rect : coveringRectangles) {
            if (!this->overlap(rect)) {
                 // Ignore completely outside rectangles
                continue;
            }
    
            events.push_back({std::max(rect._left, this->_left), std::max(rect._bottom, this->_bottom), std::min(rect._top, this->_top), 1});
            events.push_back({std::min(rect._right, this->_right), std::max(rect._bottom, this->_bottom), std::min(rect._top, this->_top), -1});
        }
    
        // Sort events by x-coordinate
        std::sort(events.begin(), events.end());
    
        int prevX = this->_left;
        int coveredArea = 0;
        std::multiset<std::pair<int, int>> activeIntervals; // Stores active y-intervals
    
        for (const auto& event : events) {
            int currentX = event.x;
    
            // Calculate the covered vertical range
            int coveredHeight = 0;
            int lastY = -1;
    
            for (const auto& interval : activeIntervals) {
                int y1 = interval.first;
                int y2 = interval.second;
    
                if (lastY < y1) {
                    coveredHeight += (y2 - y1);
                } else {
                    coveredHeight += std::max(0, y2 - lastY);
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

        return (double)coveredArea / this->area();;
    }

    bool contains(const WebXRectangle & rectangle) const {
        return (
            this->_left <= rectangle._left &&
            this->_right >= rectangle._right &&
            this->_top >= rectangle._top &&
            this->_bottom <= rectangle._bottom);
    }

    void clear() {
        this->x = 0;
        this->y = 0;
        this->size = WebXSize();
        this->_left = 0;
        this->_right = 0;
        this->_top = 0;
        this->_bottom = 0;
    }

    int area() const {
        return this->size.area();
    }

    WebXRectangle & operator+=(const WebXRectangle & rectangle) {

        int left = rectangle._left < this->_left ? rectangle._left : this->_left;
        int right = rectangle._right > this->_right ? rectangle._right : this->_right;
        int bottom = rectangle._bottom < this->_bottom ? rectangle._bottom : this->_bottom;
        int top = rectangle._top > this->_top ? rectangle._top : this->_top;

        this->x = left;
        this->y = bottom;
        this->size.width = right - left;
        this->size.height = top - bottom;

        this->_left = left;
        this->_right = right;
        this->_bottom = bottom;
        this->_top = top;

        return *this;
    }

    int x;
    int y;
    WebXSize size;

private:
    int _left;
    int _right;
    int _top;
    int _bottom;
};


#endif /* WEBX_RECTANGLE_H */