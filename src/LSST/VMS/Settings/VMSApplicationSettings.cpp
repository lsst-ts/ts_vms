/*
 * VMSApplicationSettings.cpp
 *
 *  Created on: Sep 29, 2017
 *      Author: ccontaxis
 */

#include <VMSApplicationSettings.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <pugixml.hpp>

namespace LSST {
namespace VMS {

void VMSApplicationSettings::load(const std::string &filename) {
  pugi::xml_document doc;
  doc.load_file(filename.c_str());

  this->Subsystem = doc.select_node("//VMSApplicationSettings/Subsystem")
                        .node()
                        .child_value();
  this->IsMaster = boost::lexical_cast<int>(
                       doc.select_node("//VMSApplicationSettings/IsMaster")
                           .node()
                           .child_value()) != 0;
  this->NumberOfSensors = boost::lexical_cast<int>(
      doc.select_node("//VMSApplicationSettings/NumberOfSensors")
          .node()
          .child_value());
  this->loadVector(&this->XCoefficients,
                   doc.select_node("//VMSApplicationSettings/XCoefficients")
                       .node()
                       .child_value());
  this->loadVector(&this->YCoefficients,
                   doc.select_node("//VMSApplicationSettings/YCoefficients")
                       .node()
                       .child_value());
  this->loadVector(&this->ZCoefficients,
                   doc.select_node("//VMSApplicationSettings/ZCoefficients")
                       .node()
                       .child_value());
  this->loadVector(&this->XOffsets,
                   doc.select_node("//VMSApplicationSettings/XOffsets")
                       .node()
                       .child_value());
  this->loadVector(&this->YOffsets,
                   doc.select_node("//VMSApplicationSettings/YOffsets")
                       .node()
                       .child_value());
  this->loadVector(&this->ZOffsets,
                   doc.select_node("//VMSApplicationSettings/ZOffsets")
                       .node()
                       .child_value());
}

void VMSApplicationSettings::loadVector(std::vector<double> *vector,
                                        std::string text) {
  typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;
  tokenizer tokenize(text);
  for (tokenizer::iterator token = tokenize.begin(); token != tokenize.end();
       ++token) {
    vector->push_back(boost::lexical_cast<double>(*token));
  }
}

} /* namespace VMS */
} /* namespace LSST */
