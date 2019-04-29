
//
//  We need the top level directory in a few places below, because we work
//  in terms of fully qualified URLs. We want to be flexible in where the
//  help is placed, so that we can post preview versions or old product
//  versions, so we get it from the initial incoming URL.
//
var helpRoot = null;


// -------------------------------------------------------------------
//  Handle initialization on the main page. We need to get the main
//  generated content page loaded. There is some basic text in it, but
//  we need to get the main (top level) topic list loaded into the left
//  hand side div.
// -------------------------------------------------------------------
function doInit()
{
    //
    //  We need the top level directory in a few places below, because we work
    //  in terms of fully qualified URLs. We want to be flexible in where the
    //  help is placed, so that we can post preview versions or old product
    //  versions, so we get it from the initial incoming URL.
    //
    //  NOTE: For now it still doesn't work because of image references. We don't
    //  want to hard code relative paths to the images, so until we can update the
    //  docs compiler to generate the relative paths, we still have to only host this
    //  help content under /Web2/CIDDocs
    //
    helpRoot = "/Web2/CIDDocs";

    // See if we have been asked to do debug mode
    debugCQSL = getParameterByName("debug");

    //
    //  If asked to, disable Ajax caching. WE have a debug introductory page that
    //  will set this.
    //
    if (debugCQSL)
    {
        $.ajaxSetup({ cache: false });
    }

    //
    //  Get the invocation URL. If there are no query parms for a page to load, we load
    //  the default initial page. Else we load the indicated one.
    //
    var tarTopic = getParameterByName("topic");
    var tarPage = getParameterByName("page");
    var tarBookMark = getParameterByName("bookmark");

    if (!tarTopic && !tarPage)
    {
        tarTopic = "/";
        tarPage = "/Overview";
    }
    if (!tarBookMark)
        tarBookMark = "";

    // Start us off with the initial location on the history stack
    if (history.replaceState) {

        var stateObj = {
            topic : tarTopic,
            page : tarPage,
            bookmark : tarBookMark
        };

        var initURL = helpRoot + "/CIDDocs.html?topic="
                      + tarTopic + "&page=" + tarPage;
        if (tarBookMark !== "")
            initURL += "&bookmark=" + tarBookMark;

        history.replaceState(stateObj, null, initURL);
    }

    // Build up the left and right side URLs
    var pagePath= tarTopic + "/" + tarPage;
    var topicURL = helpRoot + tarTopic + "/CIDTopicIndex.html";
    var pageURL = helpRoot + pagePath + ".html";

    //
    //  Make sure no double slashes. Do the page path twice since it can end up
    //  with three slashes.
    //
    pagePath = pagePath.replace(/\/\//g, '/');
    pagePath = pagePath.replace(/\/\//g, '/');
    topicURL = topicURL.replace(/\/\//g, '/');
    pageURL = pageURL.replace(/\/\//g, '/');

    // Load the initial content when our document is ready.
    $(document).ready
    (
        function(){
            // Get the initial introduction page loaded
            $("#RightSide").load(pageURL, function()
            {
                document.getElementById("CurHelpPath").innerText = pagePath;

                if (tarBookMark !== "") {
                    var bmElem = document.getElementById(tarBookMark);
                    if (bmElem)
                        bmElem.scrollIntoView();
                }
            });

            $("#LeftSide").load(topicURL, function()
            {
            });
        }
    );

    //
    //  Set up a history state pop handler so that we can reload the right stuff when
    //  the user uses the back button.
    //
    window.onpopstate = function(event) {

        var newState = event.state;
        if (newState) {

            // It's a back operation, so go back
            var newTopic = newState["topic"];
            var newPage = newState["page"];
            var newBookMark = newState["bookmark"];

            var pagePath= newTopic + "/" + newPage;
            var topicURL = helpRoot + newTopic + "/CIDTopicIndex.html";
            var pageURL = helpRoot + pagePath + ".html";

            pagePath = pagePath.replace(/\/\//g, '/');
            pagePath = pagePath.replace(/\/\//g, '/');
            topicURL = topicURL.replace(/\/\//g, '/');
            pageURL = pageURL.replace(/\/\//g, '/');

            $("#RightSide").load(pageURL, function()
            {
                document.getElementById("CurHelpPath").innerText = pagePath;

                if (newBookMark !== "") {
                    var bmElem = document.getElementById(newBookMark);
                    if (bmElem)
                        bmElem.scrollIntoView();
                }
            });

            // If the new path is different from the current path, load the index
            $("#LeftSide").load(topicURL, function()
            {
            });
        }
    };
}


// -------------------------------------------------------------------
//  This is a helper that we can use to get to a particular query parameter on the URL that
//  we were invoked with.
//
//  Because the parameters we get are well defined and simple, we can use a simple method
//  to get them out. Basically we split once on &, and for each of those we split on =. None
//  of our values need to be decoded or have spaces in them. And since we only look at the
//  search part, the fact that they have forward slashes in them is not ambiguous. We never
//  see the main part of the URL.
// -------------------------------------------------------------------
function getParameterByName(name)
{
    // Get a copy of the query string
    var searchStr = window.location.search;

    // If it happens they don't support that, let's get out of the full URL
    if (!searchStr)
    {
        if (window.location.href)
        {
            var splitList = window.location.href.split('?');
            if (splitList && (splitList.length === 2))
            searchStr = splitList[1];
        }
    }

    // Optimize if no query string at all
    if (!searchStr || (searchStr.length === 0))
        return null;

    // Get rid of the leading ? if it is present
    if (searchStr[0] === '?')
        searchStr = searchStr.substr(1);

    // Split that now for our outer list
    var outerList = searchStr.split('&');
    var retVal = null;

    for (i = 0; i < outerList.length; i++)
    {
        var curParm = outerList[i].split('=');

        // If we got anything, and it's for the one we want
        if (curParm && (curParm[0].toUpperCase() === name.toUpperCase()))
        {
            // If we got a value, return that
            if (curParm.length > 1)
                retVal = curParm[1];

            // Break either way
            break;
        }
    }
    return retVal;
}



//
//  Called from topic index links that link to current level files. We just
//  load the indicated page into the right side DIV.
//
function loadRightSide(topicDir, pageToLoad, bookMark)
{
    // Set up an undo history state for thew new page
    if (history.pushState) {
        var stateObj = {
            topic : topicDir,
            page : pageToLoad,
            bookmark : bookMark
        };

        var newURL = helpRoot + "/CIDDocs.html?topic="
                      + topicDir + "&page=" + pageToLoad;

        if (bookMark !== "")
            newURL += "&bookmark=" + bookMark;

        history.pushState(stateObj, "CID Topic", newURL);
    }

    var pagePath = topicDir + "/" + pageToLoad
    var newFile = helpRoot + pagePath + ".html";

    pagePath = pagePath.replace(/\/\//g, '/');
    pagePath = pagePath.replace(/\/\//g, '/');
    newFile = newFile.replace(/\/\//g, '/');

    $(document).ready
    (
        function(){
            $("#RightSide").load(newFile, function()
            {
                document.getElementById("CurHelpPath").innerText = pagePath;

                if (bookMark !== "") {
                    var bmElem = document.getElementById(bookMark);
                    if (bmElem)
                        bmElem.scrollIntoView();
                }
            });
        }
    );
    return true;
}


//
//  Called from topic index links that link to a sub-topic (or back up to the
//  parent topic.) We load the main topic page for that topic into the right side
//  div, and the topic index fro that topic into the left side div.
//
//  It's also called from a click on a generated page type link, which will pass in
//  the same info.
//
function loadDynDiv(topicDir, topicPage)
{
    // Push an undo history state for this new page
    if (history.pushState) {
        var stateObj = {
            topic : topicDir,
            page : topicPage,
            bookmark : ""
        };

        var newURL = helpRoot + "/CIDDocs.html?topic="
                     + topicDir + "&page=" + topicPage;
        history.pushState(stateObj, "CID Topic", newURL);
    }

    var pagePath = topicDir + "/" + topicPage;
    var topicURL = helpRoot + topicDir + "/CIDTopicIndex.html";
    var pageURL = helpRoot + pagePath + ".html";

    pagePath = pagePath.replace(/\/\//g, '/');
    pagePath = pagePath.replace(/\/\//g, '/');
    topicURL = topicURL.replace(/\/\//g, '/');
    pageURL = pageURL.replace(/\/\//g, '/');

    $(document).ready
    (
        function(){
            $("#RightSide").load(pageURL, function()
            {
                document.getElementById("CurHelpPath").innerText = pagePath;
            });

            $("#LeftSide").load(topicURL, function()
            {
            });
        }
    );
    return true;
}


// -------------------------------------------------------------------
//  Kick off the initialization
// -------------------------------------------------------------------
window.onload=doInit;

