#include <iostream>
#include "cavc/polylineoffset.hpp"
#include "cavc/polylineoffsetislands.hpp"

#include <sstream>
#include <string>
#include <fstream>
#include <canvas.h>
#include <glm/glm.hpp>
#include "glm-aabb/aabb.hpp"

//#include <fmt/core.h>
int main()
{
    
    cavc::Polyline<double> input;

   // std::ifstream infile;
   // infile.open("../debug_loop_pts.txt");
   // std::string line;
   // //get num of vertices
   // if(infile.good())
   // {
   //     int num_v;
   //     while(!infile.eof())
   //     {

			//getline(infile, line);
   //         if(line.size()==0)
   //             break;
			//std::istringstream iss(line);
   //         iss >> num_v;
   //         for(int i=0;i<num_v;i++)
   //         {
   //             std::getline(infile,line);
   //     
   //             //parse
   //             std::istringstream iss_v(line);
   //             float x,y,b;
   //             iss_v >> x >> y >> b;

   //             //multiply 100 to solve precision problem
   //             input.addVertex(x,y,b);
   //         }
   //         //break;
   //     }
   // }

	std::vector<cavc::Polyline<double>> m_ccwLoops;
	std::vector<cavc::Polyline<double>> m_cwLoops;

    cavc::Polyline<double> outline;
	outline.addVertex(0, 0, 0);
	outline.addVertex(0, 400, 0);
	outline.addVertex(400, 400, 0);
	outline.addVertex(400, 0, 0);
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
    
    /*double step = -2.0;
    for(int i=0;i<5;i++)
    {
        if(i==0)
        {
            auto results = cavc::parallelOffset(input, step);
            lists.emplace_back(results);
        }
        else
        {
			auto results = cavc::parallelOffset(lists[lists.size()-1][0], step);
			lists.emplace_back(results);
        }
    }
    */

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

	int m_offsetCount = 40;
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

        glm::AABB aabb;
		
		for (auto& input : m_ccwLoops)
		{
			auto vertices = input.vertexes();
			for (int i = 0; i < vertices.size() ; i++)
			{
				auto v = vertices[i];
				aabb.extend(glm::vec3(v.x(),v.y(),0.0f));
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


        auto min_p = aabb.getMin();
        auto max_p = aabb.getMax();
		//fmt::print("min {0} {1}\n", min_p.x,min_p.y);
		//fmt::print("max {0} {1}\n", max_p.x, max_p.y);
        auto dis_size = max_p - min_p;
		float aspect = dis_size.x / dis_size.y;
		auto offset = glm::vec3(window_size, window_size, 0) * 0.2f;
        float dis_sizemax = std::max(dis_size.x,dis_size.y);
        float scalemax = std::max(window_size- 2*offset.x,window_size -2*offset.y);
        auto scale = glm::vec3(scalemax,scalemax,0.0f) / dis_sizemax;
        
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
					buf.draw_line(cv_s.x,cv_s.y,cv_e.x,cv_e.y,yellow);
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
					buf.draw_line(cv_s.x, cv_s.y, cv_e.x, cv_e.y, yellow);
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
				    buf.draw_line(cv_s.x, cv_s.y, cv_e.x, cv_e.y, red);
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

  //  canvas::on_render([&](){
  //      using namespace canvas::color;

  //      canvas::buffer buf;
  //      buf.set_size(800, 600, gray);

  //      glm::AABB aabb;
		//for (auto& line : results)
		//{
		//	auto vertices = line.vertexes();
		//	for (int i = 0; i < vertices.size() ; i++)
  //          {
  //              auto v = vertices[i];
  //              aabb.extend(glm::vec3(v.x(),v.y(),0.0f));
  //          }
  //      }
  //      auto min_p = aabb.getMin();
  //      auto max_p = aabb.getMax();
		////fmt::print("min {0} {1}\n", min_p.x,min_p.y);
		////fmt::print("max {0} {1}\n", max_p.x, max_p.y);
  //      auto dis_size = max_p - min_p;
  //      float aspect = dis_size.x / dis_size.y;
  //      auto scale = glm::vec3(600,600,0) / (dis_size);
  //      for(auto& line : results)
  //      {
		//	auto vertices = line.vertexes();
		//	for(int i=0;i<vertices.size()-1;i++)
		//	{
		//		auto v_s = vertices[i];
  //              auto v_e = vertices[(i+1)%vertices.size()];

		//		auto cv_s = (glm::vec3(v_s.x(), v_s.y(), 0.0) - min_p) * scale;
		//		auto cv_e = (glm::vec3(v_e.x(), v_e.y(), 0.0) - min_p) * scale;
  //              buf.draw_line(cv_s.x,cv_s.y,cv_e.x,cv_e.y,yellow);
		//	}
  //      }
  //      
  //      canvas::get_frame().stretch(buf);

  //      /*  Draw the canvas.
  //       */
  //      canvas::finalize();

  //      /*  Reset the render function.
  //       */
  //      canvas::done();
  //  });

    return canvas::run();
}