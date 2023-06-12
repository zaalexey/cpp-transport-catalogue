#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        bool first_point = true;
        out << "<polyline points=\""sv;
        for (const auto& point : points_) {
            if (first_point) {
                out << point.x << "," << point.y;
                first_point = false;
            }
            else
                out << " "sv << point.x << "," << point.y;
        }
        out << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos) {
        pos_ = pos;
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text";
        RenderAttrs(out);
        out << " x=\"" << pos_.x << "\" y=\"" << pos_.y << "\" dx=\"" << offset_.x
            << "\" dy=\"" << offset_.y << "\" font-size=\"" << font_size_;

        if (!font_family_.empty()) {
            out << "\" font-family=\"" << font_family_;
        }
        if (!font_weight_.empty()) {
            out << "\" font-weight=\"" << font_weight_;
        }
        out << "\">" << data_ << "</text>";
    }


    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }
    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
        for (const auto& object : objects_) {
            object->Render({ out, 2, 2 });
        }
        out << "</svg>";
    }



    std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap_) {
        using namespace std::literals;
        switch (static_cast<int>(line_cap_)) {
        case 0:
            out << "butt"sv;
            break;
        case 1:
            out << "round"sv;
            break;
        case 2:
            out << "square"sv;
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join_) {
        using namespace std::literals;
        switch (static_cast<int>(line_join_)) {
        case 0:
            out << "arcs"sv;
            break;
        case 1:
            out << "bevel"sv;
            break;
        case 2:
            out << "miter"sv;
            break;
        case 3:
            out << "miter-clip"sv;
            break;
        case 4:
            out << "round"sv;
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, Color& color) {
        std::visit(ColorPrinter{ out }, color);
        return out;
    }

}  // namespace svg