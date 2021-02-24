/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "senseiCloud.H"
#include "cloud.H"
#include "foamVtkCloudAdaptor.H"
#include "addToRunTimeSelectionTable.H"

#include <vtkMultiBlockDataSet.h>
#include <vtkInformation.h>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace sensei
{
    defineTypeNameAndDebug(cloudInput, 0);
    addNamedToRunTimeSelectionTable
    (
        senseiInput,
        cloudInput,
        dictionary,
        cloud
    );
}
} // End namespace Foam


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sensei::cloudInput::cloudInput
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    senseiInput(name),
    time_(runTime),
    regionName_(),
    selectClouds_(),
    selectFields_()
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::sensei::cloudInput::read(const dictionary& dict)
{
    senseiInput::read(dict);

    regionName_ =
        dict.lookupOrDefault<word>("region", polyMesh::defaultRegion);

    selectClouds_.clear();
    dict.readIfPresent("clouds", selectClouds_);

    if (selectClouds_.empty())
    {
        selectClouds_.resize(1);
        selectClouds_.first() =
            dict.lookupOrDefault<word>("cloud", cloud::defaultName);
    }

    selectFields_.clear();
    dict.readIfPresent("fields", selectFields_);

    return true;
}


Foam::label Foam::sensei::cloudInput::addChannels(dataQuery& dataq)
{
    const fvMesh& fvm = time_.lookupObject<fvMesh>(regionName_);

    if (fvm.names<cloud>(selectClouds_).empty())
    {
        return 0;
    }

    // Difficult to get the names of the fields from particles
    // ... need to skip for now
    wordHashSet allFields;

    dataq.set(name(), allFields);

    return 1;
}


bool Foam::sensei::cloudInput::convert
(
    dataQuery& dataq,
    outputChannels& outputs
)
{
    const fileName channelName(name());
    const fvMesh& fvm = time_.lookupObject<fvMesh>(regionName_);
    const wordList cloudNames(fvm.sortedNames<cloud>(selectClouds_));

    if (cloudNames.empty() || !dataq.found(channelName))
    {
        // Not available, or not requested
        return false;
    }


    // A separate block for each cloud
    unsigned int blockNo = 0;

    for (const word& cloudName : cloudNames)
    {
        auto dataset =
            vtk::cloudAdaptor(fvm).getCloud(cloudName, selectFields_);

        // Existing or new
        vtkSmartPointer<vtkMultiBlockDataSet> block =
            outputs.lookup
            (
                channelName,
                vtkSmartPointer<vtkMultiBlockDataSet>::New()
            );

        block->SetBlock(blockNo, dataset);

        block->GetMetaData(blockNo)->Set
        (
            vtkCompositeDataSet::NAME(),
            cloudName                       // block name = cloud name
        );

        outputs.set(channelName, block);    // overwrites existing

        ++blockNo;
    }

    return true;
}


Foam::Ostream& Foam::sensei::cloudInput::print(Ostream& os) const
{
    os  << name() << nl
        <<"    clouds  " << flatOutput(selectClouds_) << nl
        <<"    fields  " << flatOutput(selectFields_) << nl;

    return os;
}


// ************************************************************************* //
