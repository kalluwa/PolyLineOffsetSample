#include <iostream>
#include "cavc/polylineoffset.hpp"
#include "cavc/polylineoffsetislands.hpp"

#include <sstream>
#include <string>
#include <fstream>
#include <canvas.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "glm-aabb/aabb.hpp"

//#include <fmt/core.h>
int main()
{
    
    cavc::Polyline<double> input;

	std::vector<cavc::Polyline<double>> m_ccwLoops;
	std::vector<cavc::Polyline<double>> m_cwLoops;

    cavc::Polyline<double> outline;
	outline.addVertex(0, 0, 0);
	outline.addVertex(0, 300, 0);
	outline.addVertex(300, 300, 0);
	outline.addVertex(300, 0, 0);
    outline.isClosed() = true;
    cavc::invertDirection(outline);
    m_ccwLoops.push_back(outline);


    float xoff = 100;
    float yoff = 100;
    cavc::Polyline<double> island;
	island.addVertex(xoff, yoff, 0);
	island.addVertex(xoff + 100, yoff, 0);
	island.addVertex(xoff+100, 100+yoff, 0);
	island.addVertex(xoff, 100+yoff, 0);
	island.isClosed() = true;
    cavc::invertDirection(island);
    m_cwLoops.emplace_back(island);


	//no exception
	input = outline;
    // compute the resulting offset polylines, offset = 3
    std::vector< std::vector<cavc::Polyline<double>>> lists;


	cavc::ParallelOffsetIslands<double> alg;
	cavc::OffsetLoopSet<double> loopSet;
    
    //fill data
    for(auto const& loop : m_ccwLoops)
	{
		loopSet.ccwLoops.push_back({ 0, loop, createApproxSpatialIndex(loop) });
    }
    for(auto const& loop : m_cwLoops)
    {
        loopSet.cwLoops.push_back({0,loop,createApproxSpatialIndex(loop)});
    }

	int m_offsetCount = 15;
	float m_offsetDelta = 5.0;
	int i = 0;

	while (i < m_offsetCount) {
		loopSet = alg.compute(loopSet, m_offsetDelta);
		if (loopSet.cwLoops.size() == 0 && loopSet.ccwLoops.size() == 0) {
			break;
		}

#if 1
		std::vector<cavc::Polyline<double>> lines;
		for (auto const& loop : loopSet.cwLoops) {
			lines.emplace_back(loop.polyline);
		}
		for (auto const& loop : loopSet.ccwLoops) {
			lines.emplace_back(loop.polyline);
		}

		lists.emplace_back(lines);
#endif
		i += 1;
	}
    //draw all
	canvas::set_title(L"polyline Example");
    int window_size=600;
	canvas::set_size(window_size, window_size);

        canvas::on_render([&](){
        using namespace canvas::color;

        canvas::buffer buf;
        buf.set_size(window_size, window_size, gray);


#pragma region get boundingbox of shape

		glm::AABB aabb;

		for (auto& input : m_ccwLoops)
		{
			auto vertices = input.vertexes();
			for (int i = 0; i < vertices.size(); i++)
			{
				auto v = vertices[i];

				aabb.extend(glm::vec3(v.x(), v.y(), 0.0f));
			}
		}
		for (auto& input : m_cwLoops)
		{
			auto vertices = input.vertexes();
			for (int i = 0; i < vertices.size(); i++)
			{
				auto v = vertices[i];
				aabb.extend(glm::vec3(v.x(), v.y(), 0.0f));
			}
		}

#pragma endregion

		//get corner points
        auto min_p = aabb.getMin();
        auto max_p = aabb.getMax();

		//fmt::print("min {0} {1}\n", min_p.x, min_p.y);
		//fmt::print("max {0} {1}\n", max_p.x, max_p.y);
        auto dis_size = max_p - min_p;
		float aspect = dis_size.x / dis_size.y;
		auto offset = glm::vec3(window_size, window_size, 0) * 0.2f;
        float dis_sizemax = std::max(dis_size.x,dis_size.y);
        float scalemax = std::max(window_size- 2*offset.x,window_size -2*offset.y);
        auto scale = glm::vec3(scalemax,scalemax,0.0f) / dis_sizemax;
        

		//https://zhuanlan.zhihu.com/p/348798153
		auto draw_arc = [&](canvas::buffer& buf,const glm::f64vec2& pt0,double bulge0,const glm::f64vec2& pt1,unsigned int color) {
			//it's just a line
			if (abs(bulge0) < 0.0000001) {
				auto p0 = (glm::vec3(pt0.x, pt0.y, 0) - min_p);
				auto p1 = (glm::vec3(pt1.x, pt1.y, 0) - min_p);
				buf.draw_line(p0.x, p0.y, p1.x, p1.y, color);
				return;
			}
			//calculate the center of arc
			double b = (1 / bulge0 - bulge0) / 2;
			glm::vec2 center(
				(pt0.x + pt1.x - (pt1.y - pt0.y) * b) / 2,
				(pt0.y + pt1.y + (pt1.x - pt0.x) * b) / 2
			);

			//get radius
			double r = sqrt(pow((pt0.x - center.x), 2) + pow((pt0.y - center.y), 2));


			//get section angles
			double angle_s = 0.0;
			if (pt0.x -center.x >= 0 && pt0.y -center.y >= 0) {
				// 第一象限（以下算式包含正弦及弧度转角度）
				angle_s = asin((pt0.y - center.y) / r) / (glm::pi<double>() / 180);
			}
			else if (pt0.x -center.x <= 0 && pt0.y -center.y >=0) {
				// 第二象限
				angle_s = 180 - asin((pt0.y - center.y) / r) / (glm::pi<double>() / 180);
			}
			else if (pt0.x -center.x <= 0 && pt0.y -center.y <= 0) {
				// 第三象限
				angle_s = 180 - asin((pt0.y - center.y) / r) / (glm::pi<double>() / 180);
			}
			else if (pt0.x -center.x >= 0 && pt0.y -center.y <= 0) {
				// 第四象限
				angle_s = 360 + asin((pt0.y - center.y) / r) / (glm::pi<double>() / 180);
			}

			double angle_e = 0.0;
			if (pt1.x - center.x >= 0 && pt1.y - center.y >= 0) {
				// 第一象限（以下算式包含正弦及弧度转角度）
				angle_e = asin((pt1.y - center.y) / r) / (glm::pi<double>() / 180);
			}
			else if (pt1.x - center.x <= 0 && pt1.y - center.y >= 0) {
				// 第二象限
				angle_e = 180 - asin((pt1.y - center.y) / r) / (glm::pi<double>() / 180);
			}
			else if (pt1.x - center.x <= 0 && pt1.y - center.y <= 0) {
				// 第三象限
				angle_e = 180 - asin((pt1.y - center.y) / r) / (glm::pi<double>() / 180);
			}
			else if (pt1.x - center.x >= 0 && pt1.y - center.y <= 0) {
				// 第四象限
				angle_e = 360 + asin((pt1.y - center.y) / r) / (glm::pi<double>() / 180);
			}

			//draw point every 5 degree
			int step = 5;
			std::vector<glm::f64vec2> pts;
			pts.emplace_back(pt0);
			//check the bulge is positive(CCW) or negative (CW)
			if (bulge0 > 0)
			{
				if (angle_s > angle_e) {
					angle_e += 360;
				}

				//sample points
				for (auto i = step; i <= int(angle_e - angle_s); i+=step) {
					pts.emplace_back(glm::f64vec2(
						center.x + r * cos((angle_s + i) * (glm::pi<double>() / 180)),
						center.y + r * sin((angle_s + i) * (glm::pi<double>() / 180))
					));
				}
			}
			else//(bulge0 < 0)
			{
				if (angle_s < angle_e)
					angle_s += 360;

				//sample
				for (auto i = step; i <= int(angle_s - angle_e); i+=step) {
					pts.emplace_back(glm::f64vec2(
						center.x + r * cos((angle_s - i) * (glm::pi<double>() / 180)),
						center.y + r * sin((angle_s - i) * (glm::pi<double>() / 180))
					));
				}
			}
			//push end point
			pts.emplace_back(pt1);

			//draw this arc
			for (int i = 0; i < pts.size() - 1; i++)
			{
				auto p0 =  (glm::vec3(pts[i].x, pts[i].y, 0) - min_p) ;
				auto p1 =  (glm::vec3(pts[i + 1].x, pts[i + 1].y, 0) - min_p);
				buf.draw_line(p0.x, p0.y, p1.x, p1.y, color);
			}
		};
		//draw outline and inner line
        {
			for(auto& input : m_ccwLoops)
			{
				auto vertices = input.vertexes();
				for(int i=0;i<vertices.size();i++)
				{
					auto v_s = vertices[i];
					auto v_e = vertices[(i+1)%vertices.size()];

					auto cv_s = offset + (glm::vec3(v_s.x(), v_s.y(), 0.0) - min_p) * scale;
					auto cv_e = offset + (glm::vec3(v_e.x(), v_e.y(), 0.0) - min_p) * scale;

					auto bulge_s = v_s.bulge();
					auto bulge_e = v_e.bulge();


					//buf.draw_line(cv_s.x,cv_s.y,cv_e.x,cv_e.y,yellow);

					//draw_arc(buf, cv_s, bulge_s, cv_e, yellow);

					draw_arc(buf, { v_s.x(),v_s.y() }, v_s.bulge(), { v_e.x(),v_e.y() }, yellow);
				}
			}
			for (auto& input : m_cwLoops)
			{
				auto vertices = input.vertexes();
				for (int i = 0; i < vertices.size(); i++)
				{
					auto v_s = vertices[i];
					auto v_e = vertices[(i + 1) % vertices.size()];

					auto cv_s = offset + (glm::vec3(v_s.x(), v_s.y(), 0.0) - min_p) * scale;
					auto cv_e = offset + (glm::vec3(v_e.x(), v_e.y(), 0.0) - min_p) * scale;
					//buf.draw_line(cv_s.x, cv_s.y, cv_e.x, cv_e.y, yellow);

					auto bulge_s = v_s.bulge();

					//draw_arc(buf, cv_s, bulge_s, cv_e, yellow);

					draw_arc(buf, { v_s.x(),v_s.y() }, v_s.bulge(), { v_e.x(),v_e.y() }, yellow);
				}
			}
        }
        for(auto lines : lists)
        {
            for(auto& line : lines)
		    {
			    auto vertices = line.vertexes();
			    for (int i = 0; i < vertices.size(); i++)
			    {
				    auto v_s = vertices[i];
				    auto v_e = vertices[(i + 1) % vertices.size()];

				    auto cv_s =offset +  (glm::vec3(v_s.x(), v_s.y(), 0.0) - min_p) * scale;
				    auto cv_e =offset +  (glm::vec3(v_e.x(), v_e.y(), 0.0) - min_p) * scale;
				    //buf.draw_line(cv_s.x, cv_s.y, cv_e.x, cv_e.y, red);

					auto bulge_s = v_s.bulge();

					//draw_arc(buf, cv_s, bulge_s, cv_e,red);

					draw_arc(buf, { v_s.x(),v_s.y() }, v_s.bulge(), { v_e.x(),v_e.y() }, red);
			    }
		    }
        }
        
        canvas::get_frame().stretch(buf);

        /*  Draw the canvas.
         */
        canvas::finalize();

        /*  Reset the render function.
         */
        canvas::done();
    });


    return canvas::run();
}